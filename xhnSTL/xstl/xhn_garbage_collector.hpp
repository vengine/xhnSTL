/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef __Xgc__xhn_garbage_collector__
#define __Xgc__xhn_garbage_collector__

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"

#include "xhn_memory.hpp"
#include "xhn_vector.hpp"
#include "xhn_map.hpp"
#include "xhn_set.hpp"
#include "timer.h"

#define MAX_GENERATIONS 3

///#define GC_DEBUG
#ifdef GC_DEBUG
static xhn::set<vptr> s_memset;
#endif
class GCNode : public MemObject
{
public:
    TimeCheckpoint birthTime;
    euint32 retainCounts[MAX_GENERATIONS];
    esint32 generationIndex;
    xhn::set<GCNode*> attachedNodes;
    GCNode* prev;
    GCNode* next;
    vptr memory;
    euint64 size;
    bool isRoot;
    bool isAlived;
    GCNode()
    : generationIndex(MAX_GENERATIONS - 1)
    , prev(NULL)
    , next(NULL)
    , memory(NULL)
    , size(0)
    , isRoot(false)
    , isAlived(false)
    {
        memset(retainCounts, 0, sizeof(retainCounts));
#ifdef GC_DEBUG
        s_memset.insert(this);
#endif
    }
    ~GCNode() {
#ifdef GC_DEBUG
        s_memset.erase(this);
#endif
        if (memory) {
            Mfree(memory);
            memory = NULL;
        }
    }
};

class GCRootNode : public GCNode
{
public:
    GCRootNode* rootPrev;
    GCRootNode* rootNext;
    GCRootNode()
    : rootPrev(NULL)
    , rootNext(NULL)
    {}
};

class Generation : public MemObject
{
    friend class GarbageCollector;
private:
    const esint32 generationIndex;
    GCNode* memHead;
    GCNode* memTail;
    GCRootNode* rootHead;
    GCRootNode* rootTail;
    xhn::vector<GCNode*> buffer0;
    xhn::vector<GCNode*> buffer1;
    TimeCheckpoint currentTime;
    euint64 timeLimit;
    euint64 totalSize;
public:
    Generation(esint32 genIndex, euint64 limit)
    : generationIndex(genIndex)
    , memHead(NULL)
    , memTail(NULL)
    , rootHead(NULL)
    , rootTail(NULL)
    , timeLimit(limit)
    , totalSize(0)
    {}
public:
#ifdef GC_DEBUG
    void TestMem(vptr mem)
    {
        if (s_memset.find(mem) == s_memset.end()) {
            printf("here\n");
        }
    }
#endif
    void RetainCountDecrement(GCNode* node) {
        xhn::set<GCNode*>::iterator iter = node->attachedNodes.begin();
        xhn::set<GCNode*>::iterator end = node->attachedNodes.end();
        for (; iter != end; iter++) {
            GCNode* attachedNode = *iter;
#ifdef GC_DEBUG
            TestMem(attachedNode);
#endif
            attachedNode->retainCounts[generationIndex]--;
        }
    }
    void RetainCountIncrement(GCNode* node) {
        xhn::set<GCNode*>::iterator iter = node->attachedNodes.begin();
        xhn::set<GCNode*>::iterator end = node->attachedNodes.end();
        for (; iter != end; iter++) {
            GCNode* attachedNode = *iter;
#ifdef GC_DEBUG
            TestMem(attachedNode);
#endif
            attachedNode->retainCounts[generationIndex]++;
        }
    }
    void PushRootNode(GCRootNode* rootNode) {
#ifdef GC_DEBUG
        TestMem(rootNode);
#endif
        if (rootTail) {
            rootTail->rootNext = rootNode;
            rootNode->rootPrev = rootTail;
            rootTail = rootNode;
        }
        else {
            rootHead = rootTail = rootNode;
        }
    }
    void PushNode(GCNode* node) {
#ifdef GC_DEBUG
        TestMem(node);
#endif
        if (memTail) {
            memTail->next = node;
            node->prev = memTail;
            memTail = node;
        }
        else {
            memHead = memTail = node;
        }
    }
    void EraseNotFree(GCNode* node) {
        if (node == memHead) { memHead = node->next; }
        if (node == memTail) { memTail = node->prev; }
        if (node->prev) { node->prev->next = node->next; }
        if (node->next) { node->next->prev = node->prev; }

        if (node->isRoot) {
            GCRootNode* rootNode = static_cast<GCRootNode*>(node);
            if (rootNode == rootHead) { rootHead = rootNode->rootNext; }
            if (rootNode == rootTail) { rootTail = rootNode->rootPrev; }
            if (rootNode->rootPrev) { rootNode->rootPrev->rootNext = rootNode->rootNext; }
            if (rootNode->rootNext) { rootNode->rootNext->rootPrev = rootNode->rootPrev; }
        }
        totalSize -= node->size;
    }
    void Aging(Generation* olderGen) {
        currentTime = TimeCheckpoint::Tick();
        GCNode* node = memHead;
        while (node) {
            GCNode* next = node->next;
#if defined(LINUX)
            if (TimeCheckpoint::CalcElapsedTimeInNano(node->birthTime, currentTime) > timeLimit) {
#else
            if (currentTime.timeStamp - node->birthTime.timeStamp > timeLimit) {
#endif
                RetainCountDecrement(node);
                EraseNotFree(node);
                if (node->isRoot) {
                    GCRootNode* rootNode = static_cast<GCRootNode*>(node);
                    rootNode->rootNext = rootNode->rootPrev = NULL;
                    olderGen->PushRootNode(rootNode);
                }
                node->next = node->prev = NULL;
                olderGen->PushNode(node);
                node->generationIndex = olderGen->generationIndex;
                olderGen->RetainCountIncrement(node);
                olderGen->totalSize += node->size;
            }
            else {
                break;
            }
            node = next;
        }
    }
    void Erase(GCNode* node) {
        EraseNotFree(node);
        RetainCountDecrement(node);
    }
    GCRootNode* AllocRoot(euint64 size = 0) {
        GCRootNode* ret = VNEW GCRootNode;
        PushRootNode(ret);
        PushNode(ret);
        ret->isRoot = true;
        if (size) {
            ret->memory = NMalloc(size);
        }
        ret->size = size + sizeof(GCRootNode);
        ret->birthTime = currentTime;
        totalSize += ret->size;
        return ret;
    }
    GCNode* AllocNode(euint64 size) {
        GCNode* ret = VNEW GCNode;
        PushNode(ret);
        ret->isRoot = false;
        ret->memory = NMalloc(size);
        ret->size = size + sizeof(GCNode);
        ret->birthTime = currentTime;
        totalSize += ret->size;
        return ret;
    }
    vptr AllocMem(euint64 size, GCNode** nodeOut = NULL) {
        currentTime = TimeCheckpoint::Tick();
        GCNode* node = AllocNode(size);
        if (nodeOut) {
            *nodeOut = node;
        }
        return node->memory;
    }
    vptr AllocRootMem(euint64 size, GCRootNode** nodeOut = NULL) {
        currentTime = TimeCheckpoint::Tick();
        GCRootNode* node = AllocRoot(size);
        if (nodeOut) {
            *nodeOut = node;
        }
        return node->memory;
    }

    void DetachRoot(GCRootNode* rootNode) {
#ifdef GC_DEBUG
        TestMem(rootNode);
#endif
        if (rootNode == rootHead) { rootHead = rootNode->rootNext; }
        if (rootNode == rootTail) { rootTail = rootNode->rootPrev; }
        if (rootNode->rootPrev) { rootNode->rootPrev->rootNext = rootNode->rootNext; }
        if (rootNode->rootNext) { rootNode->rootNext->rootPrev = rootNode->rootPrev; }
    }

    /// 一般的做法：
    /// 1.遍历所有节点，将isAlived标志标为false

    /// 2.遍历所有生存的跟节点，将生存的跟节点的isAlived标为true，
    /// 然后将生存的根节点的“引用节点”标记为isAlived放到“生存者表0”里面

    /// 3.将“生存者表0”中的所有节点的“引用节点”中isAlived为false的节点标记为isAlive＝true，
    /// 并将这些“引用节点”（原来isAlive是false，现在是true的节点）放入“生存者表1”中

    /// 4.交换“生存者表0”和“生存者表1”重复以上操作，直到生存者表中没有节点为止

    /// 5.遍历所有节点，将isAlived标志为false的节点加以回收即可

    /// 分代收集的做法：
    /// 1.遍历所有节点，将没有被“较老的代”所引用的节点的isAlived标志标为false，
    /// 如果节点被“较老的代”所引用，则将isAlived标为true

    /// 2.遍历所有生存的跟节点，将生存的跟节点的isAlived标为true，
    /// 然后将生存的根节点的“引用节点”之中”所处代“和“当前代”相同的节点标记为isAlived放到“生存者表0”里面，
    /// 对于和“当前代”不同的“引用节点”既不标记，也不放入“生存者表0”

    /// 3.将“生存者表0”中的所有节点的“引用节点”中isAlived为false的节点标记为isAlive＝true，
    /// 并将这些“引用节点”（原来isAlive是false，现在是true的节点）的“引用节点”放入“生存者表1”中
    /// 同样的，以上操作也是剔除“所处代”的“当前代”不同的“引用节点”

    /// 4.交换“生存者表0”和“生存者表1”重复以上操作，直到生存者表中没有节点为止

    /// 5.遍历所有节点，将isAlived标志为false的节点加以回收即可

    /// 那么如何知道一个节点是否被较老的代所引用呢？
    /// 那就需要一组引用计数，总共有多少代，就有多少引用计数，还需要“当前代”的索引。
    /// 当节点A attach 到节点B时，那么节点B会利用自己“当前代”的索引，从节点A的引用计数数组中取出对应的引用计数，并加一
    /// C++代码如下：
    /// A->retainCounts[B->generationIndex]++;
    ///
    /// 而对于一个节点而言，“当前代”并不是固定不变的，在一定的时间后，节点会“衰老”，从“年轻代”衰老到“老年代”
    /// 而在节点衰老的过程中，需要修改该节点所有“引用节点”的引用计数，
    /// 将所有“引用节点”的“年轻代”的引用计数减一，“年老代”引用计数加一

    /// 当“年老代”和“年轻代”分开回收时，先回收“年老代”，
    /// 其中一个年老节点将要被回收，而该节点被一个年轻节点所引用，而该年轻节点不久也将被回收，
    /// 那么在回收年老节点时，必须清除年轻节点对年老节点的引用信息，否则，年老节点被回收了，年轻节点的“输出表”中就出现了“野指针”
    /// 为了让一个节点能顺利的找到所有引用它的节点，引入“输入表”的概念

    /// “跨代标记”：

    void PrepareCollect() {

        buffer0.clear();
        buffer1.clear();

        GCNode* node = memHead;
        while (node) {
            bool forceAlived = false;
            /// 如果当前代不是永生代
            if (generationIndex > 0) {
                /// 查看该节点从永生代到比当前代老的代的所有引用计数
                for (esint32 i = 0; i <= generationIndex - 1; i++) {
                    /// 如果该节点被较老的代所引用，则该节点被强制保留
                    if (node->retainCounts[i] > 0) {
                        forceAlived = true;
                        break;
                    }
                }
            }

            if (forceAlived) {
                /// 将强制保留的节点放入buffer
                node->isAlived = true;
                buffer0.push_back(node);
            }
            else {
                /// 所有非强制保留的节点都会清除生存标志
                node->isAlived = false;
            }
            node = node->next;
        }
    }

    void MarkAlived() {
        /// 扫描所有跟节点，将跟节点放入buffer
        GCRootNode* rootNode = rootHead;
        while (rootNode) {
            if (!rootNode->isAlived) {
                rootNode->isAlived = true;
                xhn::set<GCNode*>::iterator aiter = rootNode->attachedNodes.begin();
                xhn::set<GCNode*>::iterator aend = rootNode->attachedNodes.end();
                for (; aiter != aend; aiter++) {
                    buffer0.push_back(*aiter);
                }
            }
            rootNode = rootNode->rootNext;
        }
        /// 从根节点出发扫描所有attach到的节点
        xhn::vector<GCNode*>* currentBuffer = &buffer0;
        xhn::vector<GCNode*>* nextBuffer = &buffer1;
        while (currentBuffer->size()) {
            xhn::vector<GCNode*>::iterator iter = currentBuffer->begin();
            xhn::vector<GCNode*>::iterator end = currentBuffer->end();
            for (; iter != end; iter++) {
                GCNode* node = *iter;
                node->isAlived = true;
                xhn::set<GCNode*>::iterator aiter = node->attachedNodes.begin();
                xhn::set<GCNode*>::iterator aend = node->attachedNodes.end();
                for (; aiter != aend; aiter++) {
                    if (!(*aiter)->isAlived && (*aiter)->generationIndex >= generationIndex) {
                        nextBuffer->push_back(*aiter);
                    }
                }
            }
            xhn::vector<GCNode*>* tmp = currentBuffer;
            currentBuffer = nextBuffer;
            nextBuffer = tmp;
            nextBuffer->clear();
        }
        buffer0.clear();
        GCNode* node = memHead;
        while (node) {
            if (node->generationIndex == generationIndex) {
                if (!node->isAlived) {
                    buffer0.push_back(node);
                }
            }
            node = node->next;
        }

    }

    void Collect() {
        {
            xhn::vector<GCNode*>::iterator iter = buffer0.begin();
            xhn::vector<GCNode*>::iterator end = buffer0.end();
            for (; iter != end; iter++) {
                GCNode* node = *iter;
#ifdef GC_DEBUG
                TestMem(node);
#endif
                Erase(node);
            }
        }
        {
            xhn::vector<GCNode*>::iterator iter = buffer0.begin();
            xhn::vector<GCNode*>::iterator end = buffer0.end();
            for (; iter != end; iter++) {
                GCNode* node = *iter;
                if (node->isRoot) {
                    GCRootNode* rootNode = static_cast<GCRootNode*>(node);
                    delete rootNode;
                }
                else {
                    delete node;
                }
            }
        }
        {
            buffer0.reset();
            buffer1.reset();
        }
    }
    inline euint64 GetTotalSize() const {
        return totalSize;
    }
};

class GarbageCollector : public MemObject
{
private:
    SINGLETON GarbageCollector* s_GarbageCollector;
private:
    Generation* generations[MAX_GENERATIONS];
public:
    GarbageCollector()
    {
        generations[0] = VNEW Generation(0, 0);
        generations[1] = VNEW Generation(1, 5000000);
        generations[2] = VNEW Generation(2, 200000);
    }
    STATIC_FUNC void Init();
    STATIC_FUNC void Dest();
    STATIC_FUNC GarbageCollector* Get();
public:
    vptr AllocMem(euint size, GCNode** nodeOut = NULL) {
        return generations[MAX_GENERATIONS - 1]->AllocMem(size, nodeOut);
    }
    vptr AllocRootMem(euint size, GCRootNode** nodeOut = NULL) {
        return generations[MAX_GENERATIONS - 1]->AllocRootMem(size, nodeOut);
    }

    void Attach(GCNode* parent, GCNode* child) {
        if (parent->attachedNodes.find(child) == parent->attachedNodes.end()) {
            parent->attachedNodes.insert(child);
            child->retainCounts[parent->generationIndex]++;
        }
    }
    void Detach(GCNode* parent, GCNode* child) {
        if (parent->attachedNodes.find(child) != parent->attachedNodes.end()) {
            parent->attachedNodes.erase(child);
            child->retainCounts[parent->generationIndex]--;
        }
    }
    void DetachRoot(GCRootNode* rootNode) {
        generations[rootNode->generationIndex]->DetachRoot(rootNode);
    }
    void Collect() {
        if (generations[0]->GetTotalSize() > 1024 * 1024) {
            generations[0]->PrepareCollect();
            generations[1]->PrepareCollect();
            generations[2]->PrepareCollect();

            generations[0]->MarkAlived();
            generations[1]->MarkAlived();
            generations[2]->MarkAlived();

            generations[2]->Collect();
            generations[1]->Collect();
            generations[0]->Collect();
        }
        else if (generations[1]->GetTotalSize() > 256 * 1024) {
            generations[1]->PrepareCollect();
            generations[2]->PrepareCollect();

            generations[1]->MarkAlived();
            generations[2]->MarkAlived();

            generations[2]->Collect();
            generations[1]->Collect();
            generations[1]->Aging(generations[0]);
        }
        else if (generations[2]->GetTotalSize() > 32 * 1024) {
            generations[2]->PrepareCollect();
            generations[2]->MarkAlived();
            generations[2]->Collect();
            generations[2]->Aging(generations[1]);
        }
    }
    void FullCollect() {
        generations[0]->PrepareCollect();
        generations[1]->PrepareCollect();
        generations[2]->PrepareCollect();

        generations[0]->MarkAlived();
        generations[1]->MarkAlived();
        generations[2]->MarkAlived();

        generations[2]->Collect();
        generations[1]->Collect();
        generations[0]->Collect();
    }
    void Print() {
#if BIT_WIDTH == 32
        printf("%lld, %d, %d\n", generations[0]->GetTotalSize(), generations[0]->buffer0.size(), generations[0]->buffer1.size());
        printf("%lld, %d, %d\n", generations[1]->GetTotalSize(), generations[1]->buffer0.size(), generations[1]->buffer1.size());
        printf("%lld, %d, %d\n", generations[2]->GetTotalSize(), generations[2]->buffer0.size(), generations[2]->buffer1.size());
#else
        printf("%lld, %lld, %lld\n", generations[0]->GetTotalSize(), generations[0]->buffer0.size(), generations[0]->buffer1.size());
        printf("%lld, %lld, %lld\n", generations[1]->GetTotalSize(), generations[1]->buffer0.size(), generations[1]->buffer1.size());
        printf("%lld, %lld, %lld\n", generations[2]->GetTotalSize(), generations[2]->buffer0.size(), generations[2]->buffer1.size());
#endif
    }
};

class GCObject
{
public:
    GarbageCollector* gc;
    GCNode* gcNode;
public:
    //////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize )
    {
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
    }
    void operator delete( void *p)
    {
        xhnSTLExce(xhn::InvalidMemoryFreeException, "this object cant delete operator");
    }
    //////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize, const char* file,int line )
    {
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
    }

    void operator delete( void *p, const char* file,int line )
    {
        xhnSTLExce(xhn::InvalidMemoryFreeException, "this object cant delete operator");
    }
    //////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize, void* ptr )
    {
        GarbageCollector* gc = (GarbageCollector*)ptr;
        GCNode* node = NULL;
        GCObject* obj = (GCObject*)gc->AllocMem(nSize, &node);
        obj->gc = gc;
        obj->gcNode = node;
        return obj;
    }

    void operator delete( void *p, void* ptr )
    {
        xhnSTLExce(xhn::InvalidMemoryFreeException, "this object cant delete operator");
    }
    //////////////////////////////////////////////////////////////////////////
    void* operator new[]( size_t nSize )
    {
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
    }

    void operator delete[]( void* ptr, size_t nSize )
    {
        xhnSTLExce(xhn::InvalidMemoryFreeException, "this object cant delete operator");
    }

};

struct GCMemoryInfo
{
    euint64 extraMemorySize;
    GarbageCollector* gc;
};

class GCMemory : public GCObject
{
private:
    euint8 memory[1];
public:
    void* operator new( size_t nSize, void* ptr )
    {
        GCMemoryInfo* info = (GCMemoryInfo*)ptr;
        GCNode* node = NULL;
        GCObject* obj = (GCObject*)info->gc->AllocMem(nSize + info->extraMemorySize, &node);
        obj->gc = info->gc;
        obj->gcNode = node;
        return obj;
    }
public:
    inline euint8* Get() {
        return memory;
    }
};

class GCRootObject : public GCObject
{
public:
    void* operator new( size_t nSize, void* ptr )
    {
        GarbageCollector* gc = (GarbageCollector*)ptr;
        GCRootNode* rootNode = NULL;
        GCObject* obj = (GCObject*)gc->AllocRootMem(nSize, &rootNode);
        obj->gc = gc;
        obj->gcNode = rootNode;
        return obj;
    }
};

class GCRootHandle
{
public:
    mutable GCObject* m_gcRootObject;
public:
    GCRootHandle(GarbageCollector* gc)
    {
        m_gcRootObject = new(gc) GCRootObject;
    }
    ~GCRootHandle()
    {
        m_gcRootObject->gc->DetachRoot((GCRootNode*)m_gcRootObject->gcNode);
    }
};

class GCInstance
{
private:
    GarbageCollector* m_gc;
public:
    GCInstance() {
        m_gc = GarbageCollector::Get();
    }
    ~GCInstance() {
        m_gc->Collect();
    }
    inline GarbageCollector* Get() {
        return m_gc;
    }
};

inline GCMemory* _GCAlloc(euint64 size, GCInstance& gc)
{
    GCMemoryInfo info = {size, gc.Get()};
    return new (&info) GCMemory;
}

#define GCBEGIN GCInstance __GC__; GCRootHandle __GC__ROOT__(__GC__.Get());
#define GC __GC__.Get()
#define GCROOT __GC__ROOT__
#define GCNEW new(__GC__.Get())
#define GCALLOC(s) _GCAlloc(s, __GC__)

template <typename T>
class GCPtr
{
private:
    GCObject* m_parent;
    mutable GCObject* m_gcObject;
public:
    GCPtr(GCObject* parent)
    : m_parent(parent)
    , m_gcObject(NULL)
    {}
    GCPtr(const GCRootHandle& root)
    : m_parent(root.m_gcRootObject)
    , m_gcObject(NULL)
    {}
    inline T* operator = (T* mem) {
        if (m_gcObject) {
            m_parent->gc->Detach(m_parent->gcNode, m_gcObject->gcNode);
        }
        if (m_parent->gcNode != mem->gcNode) {
            m_parent->gc->Attach(m_parent->gcNode, mem->gcNode);
            m_gcObject = mem;
        }
        return mem;
    }
    inline T* operator = (const GCPtr& ptr) {
        return operator = ((T*)ptr.m_gcObject);
    }
    inline T* operator ->() {
        return (T*)m_gcObject;
    }
    inline const T* operator ->() const {
        return (T*)m_gcObject;
    }
};

#endif

#endif /* defined(__Xgc__xhn_garbage_collector__) */

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
