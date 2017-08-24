//
//  xhnSTLTests.m
//  xhnSTLTests
//
//  Created by 徐海宁 on 15/10/11.
//  Copyright © 2015年 徐 海宁. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <xhnSTL/xhn_thread.hpp>
#include <xhnSTL/xhn_lambda.hpp>
#include <xhnSTL/timer.h>
#include <xhnSTL/xhn_smart_ptr.hpp>
#include <xhnSTL/xhn_vector.hpp>
#include <xhnSTL/xhn_group.hpp>
#include <xhnSTL/xhn_parallel.hpp>
#include <xhnSTL/cpu.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

@interface xhnSTLTests : XCTestCase

@end

@implementation xhnSTLTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

class Test : public RefObject
{
public:
    int a;
};
typedef xhn::SmartPtr<Test> TestPtr;
typedef xhn::WeakPtr<Test> TestWeakPtr;

- (void) testWeakPtr {
    TestWeakPtr wtest;
    {
        TestPtr test = VNEW Test;
        test->a = 100;
        {
            test.ToWeakPtr(wtest);
            TestPtr t = wtest.ToStrongPtr();
            printf("t->a == %d\n", t->a);
        }
        TestPtr t = wtest.ToStrongPtr();
        printf("t.get() == %p\n", t.get());
        XCTAssert(t.get() != nullptr, "t.get() == nullptr");
    }
    {
        TestPtr t = wtest.ToStrongPtr();
        XCTAssert(t.get() == nullptr, "t.get() != nullptr");
    }
}

#include <xhnSTL/xhn_vector.hpp>
- (void) testVector1 {
    xhn::vector<int> intArray( xhn::vector<int>(0) );
    intArray.push_back(0);
    intArray.push_back(1);
    intArray.push_back(2);
    intArray.push_back(3);
    intArray.push_back(4);
    intArray.push_back(5);
    xhn::Lambda<void(int&, bool*)> proc([](int& i, bool* stop){
        printf("%d\n", i);
        *stop = true;
    });
    intArray.for_each(proc);
    
    intArray.throw_front(intArray.begin() + 2);
    
    for (auto i : intArray) {
        printf("for %d\n", i);
    }
}

- (void) testVector2 {
    xhn::vector< xhn::vector<int> > intArray;
    intArray.push_back( xhn::vector<int>() );
    intArray.push_back( xhn::vector<int>() );
    xhn::Lambda<void(xhn::vector<int>&, bool*)> proc([](xhn::vector<int>& i, bool* stop){
        *stop = true;
    });
    intArray.for_each(proc);
}

#include <xhnSTL/xhn_hash_map.hpp>
- (void)testHashMap {
    xhn::hash_map<xhn::static_string, int>* map0 = VNEW xhn::hash_map<xhn::static_string, int>();
    xhn::hash_map<xhn::string, int>* map1 = VNEW xhn::hash_map<xhn::string, int>();
    xhn::hash_map<unsigned int, int>* map2 = VNEW xhn::hash_map<unsigned int, int>();
    
    map0->insert("1", 1);
    map1->insert("1", 1);
    map2->insert(1, 1);
    
    printf("%d\n", *map0->find("1"));
    printf("%d\n", *map1->find("1"));
    printf("%d\n", *map2->find(1));
}

#include <xhnSTL/xhn_singly_linked_list.hpp>

struct SinglyLinkedListNode
{
    xhn::string name;
    SinglyLinkedListNode* m_iter_prev;
    SinglyLinkedListNode* m_iter_next;
};
- (void)testSinglyLinkedList0 {
    xhn::singly_linked_list<SinglyLinkedListNode> singlyLinkedList;
    SinglyLinkedListNode* node0 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node1 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node2 = new SinglyLinkedListNode();
    node0->name = "node0";
    node1->name = "node1";
    node2->name = "node2";
    singlyLinkedList.add(node0);
    singlyLinkedList.add(node1);
    singlyLinkedList.add(node2);
    SinglyLinkedListNode* iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(node1);
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(singlyLinkedList.begin());
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    
    delete node0;
    delete node1;
    delete node2;
}

- (void)testSinglyLinkedList1 {
    xhn::singly_linked_list<SinglyLinkedListNode> singlyLinkedList;
    SinglyLinkedListNode* node0 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node1 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node2 = new SinglyLinkedListNode();
    node0->name = "node0";
    node1->name = "node1";
    node2->name = "node2";
    singlyLinkedList.add(node0);
    singlyLinkedList.add(node1);
    singlyLinkedList.add(node2);
    SinglyLinkedListNode* iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(node0);
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(singlyLinkedList.begin());
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    
    delete node0;
    delete node1;
    delete node2;
}
- (void)testSinglyLinkedList2 {
    xhn::singly_linked_list<SinglyLinkedListNode> singlyLinkedList;
    SinglyLinkedListNode* node0 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node1 = new SinglyLinkedListNode();
    SinglyLinkedListNode* node2 = new SinglyLinkedListNode();
    node0->name = "node0";
    node1->name = "node1";
    node2->name = "node2";
    singlyLinkedList.add(node0);
    singlyLinkedList.add(node1);
    singlyLinkedList.add(node2);
    SinglyLinkedListNode* iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(node2);
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    printf("\n");
    singlyLinkedList.remove(singlyLinkedList.begin());
    iter = singlyLinkedList.begin();
    while (iter) {
        printf("%s\n", iter->name.c_str());
        iter = iter->m_iter_next;
    }
    
    delete node0;
    delete node1;
    delete node2;
}

#include <xhnSTL/xhn_dictionary.hpp>

- (void)testDictionary0
{
    xhn::dictionary<xhn::string, int> hash_table;
    hash_table.insert("abc", 1);
    hash_table.insert("123", 2);
    hash_table.insert("xyz", 3);
    
    int* abc = hash_table.find("abc");
    printf("%d\n", *abc);
    
    int* _123 = hash_table.find("123");
    printf("%d\n", *_123);
    
    int* xyz = hash_table.find("xyz");
    printf("%d\n", *xyz);
}

- (void)testDictionary1
{
    xhn::dictionary<xhn::string, int> hash_table;
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#1:%d\n", *v);
    }
}

- (void)testDictionary2
{
    xhn::dictionary<const char*, int> hash_table;
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#2:%d\n", *v);
    }
}

- (void)testDictionary3
{
    xhn::dictionary<xhn::static_string, int> hash_table;
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#2:%d\n", *v);
    }
}

- (void) testDictionary4
{
    char mbufs[100][256];
    for (int i = 0; i < 100; i++) {
        snprintf(&mbufs[i][0], 255, "abc%dde%dfgh", i, i);
    }
    int totalValue = 0;
    
    xhn::dictionary<xhn::static_string, int> hash_table;
    for (int i = 0; i < 100; i++) {
        ///char mbuf[256];
        ///snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        hash_table.insert(&mbufs[i][0], i);
        totalValue += i;
    }
    int validationValue = 0;
    xhn::dictionary<xhn::static_string, int>::iterator iter = hash_table.begin();
    xhn::dictionary<xhn::static_string, int>::iterator end = hash_table.end();
    for (; iter != end; iter++) {
        printf("%s, %d\n", iter->first.c_str(), iter->second);
        validationValue += iter->second;
    }
    printf("total value %d, validation value %d\n", totalValue, validationValue);
}

- (void) testStdMap1
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block std::map<xhn::static_string, int> std_map;
    [self measureBlock:^{
        for (int i = 0; i < num; i++) {
            std_map.insert(std::make_pair(strs[i], i));
        }
    }];
    VDELETE_ARRAY[] strs;
}

- (void) testStdMap
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block std::map<xhn::static_string, int> std_map;
    for (int i = 0; i < num; i++) {
        std_map.insert(std::make_pair(strs[i], i));
    }
    
    [self measureBlock:^{
        int value = 0;
        for (int i = 0; i < num; i++) {
            value += std_map[strs[i]];
        }
    }];
    VDELETE_ARRAY[] strs;
}

- (void) testDictionary5
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block xhn::dictionary<xhn::static_string, int>* hash_table = VNEW xhn::dictionary<xhn::static_string, int>();
    [self measureBlock:^{
        for (int i = 0; i < num; i++) {
            hash_table->insert(strs[i], i);
        }
    }];
    VDELETE hash_table;
    VDELETE_ARRAY[] strs;
}

- (void) testDictionary6
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block xhn::dictionary<xhn::static_string, int>* hash_table = VNEW xhn::dictionary<xhn::static_string, int>();
    for (int i = 0; i < num; i++) {
        hash_table->insert(strs[i], i);
    }
    [self measureBlock:^{
        int value = 0;
        for (int i = 0; i < num; i++) {
            int* v = hash_table->find(strs[i]);
            value += *v;
        }
    }];
    VDELETE hash_table;
    VDELETE_ARRAY[] strs;
}

- (void) testXhnMap1
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block xhn::map<xhn::static_string, int> xhn_map;
    [self measureBlock:^{
        for (int i = 0; i < num; i++) {
            xhn_map.insert(strs[i], i);
        }
    }];
    VDELETE_ARRAY[] strs;
}

- (void) testXhnMap2
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block xhn::map<xhn::static_string, int> xhn_map;
    for (int i = 0; i < num; i++) {
        xhn_map.insert(strs[i], i);
    }
    [self measureBlock:^{
        int value = 0;
        for (int i = 0; i < num; i++) {
            value += xhn_map[strs[i]];
        }
    }];
    VDELETE_ARRAY[] strs;
}

- (void) testStdUnorderedMap1
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block std::unordered_map<const char*, int> std_map;
    [self measureBlock:^{
        for (int i = 0; i < num; i++) {
            std_map.insert(std::make_pair(strs[i].c_str(), i));
        }
    }];
    VDELETE_ARRAY[] strs;
}

- (void) testStdUnorderedMap2
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "abc%dde%dfgh", i, i);
        strs[i] = mbuf;
    }
    __block std::unordered_map<const char*, int> std_map;
    for (int i = 0; i < num; i++) {
        std_map.insert(std::make_pair(strs[i].c_str(), i));
    }
    [self measureBlock:^{
        int value = 0;
        for (int i = 0; i < num; i++) {
            value += std_map[strs[i].c_str()];
        }
    }];
    VDELETE_ARRAY[] strs;
}

#include <xhnSTL/xhn_static_string.hpp>
- (void) testStaticString
{
    xhn::static_string aaa("aaa");
    xhn::static_string bbb("bbb");
    printf("aaa hash %d, %p\n", aaa.hash_value(), aaa.c_str());
    printf("bbb hash %d, %p\n", bbb.hash_value(), bbb.c_str());
    xhn::static_string aaa2("aaa");
    printf("aaa2 hash %d, %p\n", aaa2.hash_value(), aaa2.c_str());
}

#include <xhnSTL/xhn_cache.hpp>
- (void) testCache
{
    xhn::cache<xhn::static_string, int, 128> cache;
    cache.insert("##aaa", 1);
    cache.insert("##bbb", 2);
    cache.insert("##ccc", 3);
    cache.insert("##ddd", 4);
    cache.insert("##eee", 5);
    cache.insert("##fff", 6);
    cache.insert("##ggg", 7);
    cache.insert("##hhh", 8);
    cache.insert("##iii", 9);
    cache.insert("##jjj", 10);
    cache.insert("##kkk", 11);
    cache.insert("##lll", 12);
    cache.insert("##mmm", 13);
    cache.insert("##nnn", 14);
    cache.insert("##ooo", 15);
    cache.insert("##ppp", 16);
    cache.insert("##qqq", 17);
    cache.insert("##rrr", 18);
    cache.insert("##sss", 19);
    cache.insert("##ttt", 20);
    cache.insert("##uuu", 21);
    cache.insert("##vvv", 22);
    cache.insert("##www", 23);
    cache.insert("##xxx", 24);
    cache.insert("##yyy", 25);
    cache.insert("##zzz", 26);
    for (auto& n : cache) {
        printf("%s, %d\n", n.first.c_str(), n.second);
    }
}
/**
#include <xhnSTL/llvm_parser.hpp>

- (void) testLLVMParser
{
    xhn::_TestLLVMParser();
}

#include <xhnSTL/asm_syntax_converter.hpp>

- (void) testAsmSyntaxConverter
{
    xhn::_TestASMSyntaxConverter();
}
**/
#include <xhnSTL/xhn_string.hpp>

- (void) testString
{
    xhn::string aaa = "111222333222343443133";
    NSAssert(aaa.rfind("222") == 9, @"error %llu", aaa.rfind("222"));
    NSAssert(aaa.rfind("333") == 6, @"error %llu", aaa.rfind("333"));
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        xhn::thread_ptr thread = VNEW xhn::thread;
        while(!thread->is_running()) {}
        volatile bool completed = false;
        TimeCheckpoint tp;
        xhn::Lambda<xhn::thread::TaskStatus()> proc([&completed, &tp](){
            completed = true;
            VTime t;
            TimeCheckpoint::Tock(tp, t);
            printf("%f\n", t.GetNanosecond());
            return xhn::thread::Completed;
        });
        tp = TimeCheckpoint::Tick();
        thread->add_lambda_task(proc);
        while (!completed) {}
    }];
}

struct ThreadData
{
    euint index;
    volatile esint64 counter;
    volatile esint64 stopped;
    pthread_t tid;
};

#define NUM_THREADS 32
#define NUM_COUNTS 10000

static ThreadData datas[NUM_THREADS];
static euint waitCounter = 0;

void* ThreadProc0(void* idx) {
    euint* ptrIdx = (euint*)idx;
    euint index = *ptrIdx;
    for (int i = 0; i < NUM_COUNTS; i++) {
        esint64 c = AtomicLoad64(&datas[index].counter);
        c++;
        AtomicStore64(c, &datas[index].counter);
        pthread_yield_np();
    }
    esint64 f = AtomicLoad64(&datas[index].stopped);
    f = 1;
    AtomicStore64(f, &datas[index].stopped);
    return nullptr;
}

void TestThreads0()
{
    waitCounter = 0;
    for (euint i = 0; i < NUM_THREADS; i++) {
        datas[i].index = i;
        datas[i].stopped = 0;
        pthread_create(&datas[i].tid, nullptr, ThreadProc0, &datas[i].index);
    }
}

void WaitAllThreadStopped0() {
    for (euint i = 0; i < NUM_THREADS;) {
        esint64 f = AtomicLoad64(&datas[i].stopped);
        if (f) {
            i++;
        }
        else {
            waitCounter++;
            pthread_yield_np();
        }
    }
}

void* ThreadProc1(void* idx) {
    euint* ptrIdx = (euint*)idx;
    euint index = *ptrIdx;
    for (int i = 0; i < NUM_COUNTS; i++) {
        esint64 c = AtomicLoad64(&datas[index].counter);
        c++;
        AtomicStore64(c, &datas[index].counter);
    }
    esint64 f = AtomicLoad64(&datas[index].stopped);
    f = 1;
    AtomicStore64(f, &datas[index].stopped);
    return nullptr;
}

void TestThreads1()
{
    waitCounter = 0;
    for (euint i = 0; i < NUM_THREADS; i++) {
        datas[i].index = i;
        datas[i].stopped = 0;
        pthread_create(&datas[i].tid, nullptr, ThreadProc1, &datas[i].index);
    }
}

void WaitAllThreadStopped1() {
    for (euint i = 0; i < NUM_THREADS;) {
        esint64 f = AtomicLoad64(&datas[i].stopped);
        if (f) {
            i++;
        }
        else {
            waitCounter++;
            pthread_yield_np();
        }
    }
}

static euint Test0WaitCounters[10];
static euint Test1WaitCounters[10];
static VTime Test0Times[10];
static VTime Test1Times[10];
static int Test0Counter = 0;
static int Test1Counter = 0;

- (void) testThreadSwitch0 {
    
    [self measureBlock:^{
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        TestThreads0();
        WaitAllThreadStopped0();
        TimeCheckpoint::Tock(tp, Test0Times[Test0Counter]);
        Test0WaitCounters[Test0Counter++] = waitCounter;
    }];
}

- (void) testThreadSwitch1 {
    
    [self measureBlock:^{
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        TestThreads1();
        WaitAllThreadStopped1();
        TimeCheckpoint::Tock(tp, Test1Times[Test1Counter]);
        Test1WaitCounters[Test0Counter++] = waitCounter;
    }];
    for (int i = 0; i < 10; i++) {
        euint yield0 = NUM_COUNTS * NUM_THREADS + Test0WaitCounters[i];
        euint yield1 = Test1WaitCounters[i];
        float time0 = Test0Times[i].GetMillisecond();
        float time1 = Test1Times[i].GetMillisecond();
        float deltaTime = time0 - time1;
        euint deltaYield = yield0 - yield1;
        float perYieldTime = deltaTime / (float)deltaYield;
        printf("线程切换时间:%f毫秒\n", perYieldTime);
    }
}

- (void) testCPU
{
    euint32 numcpus = number_of_physicalcores();
    NSLog(@"%d个物理CPU", numcpus);
    NSAssert(1, @"");
}

pthread_t s_affinitTestTid;

static volatile esint32 s_exitFlag = 0;
static volatile esint s_a;
static volatile esint s_b;
static volatile esint s_sum;
static volatile esint32 s_hasData = 0;

static float s_delayTimes[512];

void* AffinitProc(void*)
{
    /**
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    pthread_setaffinity_np(s_affinitTestTid, 8, &cpuset);
    **/
    euint64 counter = 0;
    while (counter < 1024 * 1024 * 128) {
        esint32 ret = AtomicLoad32(&s_hasData);
        if (ret) {
            s_sum = s_a + s_b;
            AtomicStore32(0, &s_hasData);
        }
        counter++;
    }
    AtomicCompareExchange(0, 1, &s_exitFlag);
    return nullptr;
}

- (void) testAffinit
{
    [self measureBlock:^{
        euint microDelayCounter = 0;
        euint smallDelayCounter = 0;
        euint mediumDelayCounter = 0;
        euint mediumHighDelayCounter = 0;
        euint highDelayCounter = 0;
        euint counter = 0;
        euint subcounter = 0;
        float nsMin = 1000000000.0f;
        float nsMax = 0.0f;
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        VTime t;
        pthread_create(&s_affinitTestTid, nullptr, AffinitProc, nullptr);
        while (!AtomicCompareExchange(1, 0, &s_exitFlag)) {
            esint32 ret = AtomicLoad32(&s_hasData);
            if (0 == ret) {
                TimeCheckpoint::Tock(tp, t);
                float ns = t.GetNanosecond();
                if (ns < nsMin) {
                    nsMin = ns;
                }
                if (ns > nsMax) {
                    nsMax = ns;
                }
                if (ns < 150.0f) {
                    microDelayCounter++;
                }
                else if (ns >= 150.0f && ns < 200.0f) {
                    smallDelayCounter++;
                }
                else if (ns >= 200.0f && ns < 10000.0f) {
                    mediumDelayCounter++;
                }
                else if (ns >= 10000.0f && ns < 100000.0f) {
                    mediumHighDelayCounter++;
                }
                else {
                    highDelayCounter++;
                }
                subcounter++;
                if (subcounter % (1024 * 2) == 0) {
                    if (counter <= 511) {
                        s_delayTimes[counter] = ns;
                        if (counter < 512) {
                            counter++;
                        }
                    }
                }
                EAssert(s_sum == s_a + s_b, "error");
                s_a = rand();
                s_b = rand();
                tp = TimeCheckpoint::Tick();
                AtomicStore32(1, &s_hasData);
            }
        }
        pthread_join(s_affinitTestTid, NULL);
        NSLog(@"最小延迟为%f纳秒, 最大延迟为%f纳秒, 微延迟数:%lld, 小延迟记数:%lld, 中等延迟记数:%lld, 中高延迟记数:%lld, 高延迟记数:%lld",
              nsMin, nsMax, microDelayCounter, smallDelayCounter, mediumDelayCounter, mediumHighDelayCounter, highDelayCounter);
        double microPercent = double(microDelayCounter) / double(microDelayCounter + smallDelayCounter + mediumDelayCounter + mediumHighDelayCounter + highDelayCounter);
        double smallPercent = double(smallDelayCounter) / double(microDelayCounter + smallDelayCounter + mediumDelayCounter + mediumHighDelayCounter + highDelayCounter);
        double microSmallPercent = double(microDelayCounter + smallDelayCounter) / double(microDelayCounter + smallDelayCounter + mediumDelayCounter + mediumHighDelayCounter + highDelayCounter);
        NSLog(@"微延迟出现机率:百分之%f", microPercent * 100.0);
        NSLog(@"小延迟出现机率:百分之%f", smallPercent * 100.0);
        NSLog(@"小微延迟出现机率:百分之%f", microSmallPercent * 100.0);
        /**
        for (euint i = 0; i < counter; i++) {
            NSLog(@"延迟时间:%f", s_delayTimes[i]);
        }
         **/
    }];
}



- (void) testNop
{
    [self measureBlock:^{
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        nanopause(1000);
        VTime t;
        TimeCheckpoint::Tock(tp, t);
        NSLog(@"pause指令%f纳秒", t.GetNanosecond());
    }];
}

- (void) testString0
{
    std::string aaa = "aabbddccddssaaggsgsjjh";
    xhn::string bbb = aaa.c_str();
    size_t pos0 = aaa.find("dds");
    euint pos1 = bbb.find("dds");
    printf("sizeof(tstring)=%ld\n", sizeof(bbb));
    XCTAssert(pos0 == pos1, @"error");
}

- (void) testStringPerformance0
{
    printf("std::string size %ld\n", sizeof(std::string));
    static char* strs[1024 * 1024];
    for (int i = 0; i < 1024 * 1024; i++) {
        char mbuf[256];
        int size = snprintf(mbuf, 255, "%d00000000000", i);
        strs[i] = (char*)malloc(size + 1);
        memcpy(strs[i], mbuf, size + 1);
    }
    [self measureBlock:^{
        size_t t = 0;
        for (int i = 0; i < 1024 * 1024; i++) {
            std::string s(strs[i]);
            t += s.size();
            t += s.find("000");
        }
    }];
    for (int i = 0; i < 1024 * 1024; i++) {
        free(strs[i]);
    }
}

- (void) testStringPerformance1
{
    printf("xhn::string size %ld\n", sizeof(std::string));
    static char* strs[1024 * 1024];
    for (int i = 0; i < 1024 * 1024; i++) {
        char mbuf[256];
        int size = snprintf(mbuf, 255, "%d00000000000", i);
        strs[i] = (char*)malloc(size + 1);
        memcpy(strs[i], mbuf, size + 1);
    }
    [self measureBlock:^{
        euint t = 0;
        for (int i = 0; i < 1024 * 1024; i++) {
            xhn::string s(strs[i]);
            t += s.size();
            t += s.find("000");
        }
    }];
    for (int i = 0; i < 1024 * 1024; i++) {
        free(strs[i]);
    }
}

- (void) testStringPerformance2
{
    static char* strs[1024 * 1024];
    for (int i = 0; i < 1024 * 1024; i++) {
        char mbuf[256];
        int size = snprintf(mbuf, 255, "%dabcdefxyzwwww%d", i, i);
        strs[i] = (char*)malloc(size + 1);
        memcpy(strs[i], mbuf, size + 1);
    }
    [self measureBlock:^{
        size_t t = 0;
        for (int i = 0; i < 1024 * 1024; i++) {
            std::string a(strs[i]);
            std::string b(strs[i]);
            std::string c = a + b;
            std::string d = c + c;
            t += d.size();
        }
    }];
    for (int i = 0; i < 1024 * 1024; i++) {
        free(strs[i]);
    }
}

- (void) testStringPerformance3
{
    static char* strs[1024 * 1024];
    for (int i = 0; i < 1024 * 1024; i++) {
        char mbuf[256];
        int size = snprintf(mbuf, 255, "%dabcdefxyzwwww%d", i, i);
        strs[i] = (char*)malloc(size + 1);
        memcpy(strs[i], mbuf, size + 1);
    }
    [self measureBlock:^{
        euint t = 0;
        for (int i = 0; i < 1024 * 1024; i++) {
            xhn::string a(strs[i]);
            xhn::string b(strs[i]);
            xhn::string c = a + b;
            xhn::string d = c + c;
            t += d.size();
        }
    }];
    for (int i = 0; i < 1024 * 1024; i++) {
        free(strs[i]);
    }
}
/**
- (void) testMmap
{
    void* ptr = mmap((void*)0x0000000100000000, 0x00000000ffffffff, PROT_NONE, MAP_FIXED, 0, 0);
    XCTAssert(ptr, @"error");
}
**/

- (void) testCachePerformance0
{
    static xhn::static_string strs[1024];
    for (int i = 0; i < 1024; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "%d", i);
        strs[i] = mbuf;
    }
    __block xhn::cache<xhn::static_string, int, 1024>* testcache = VNEW xhn::cache<xhn::static_string, int, 1024>();
    
    [self measureBlock:^{
        euint t = 0;
        for (int j = 0; j < 256; j++) {
            for (int i = 0; i < 1024; i++) {
                testcache->insert(strs[i], i);
            }
            for (int i = 0; i < 1024; i++) {
                int* v = testcache->find(strs[i]);
                t += *v;
            }
            auto iter = testcache->begin();
            auto end = testcache->end();
            for (; iter != end; iter++) {
                t += iter->second;
            }
            testcache->clear();
        }
    }];
    VDELETE testcache;
}

- (void) testStdUnorderedMapPerformance0
{
    static std::string strs[1024];
    for (int i = 0; i < 1024; i++) {
        char mbuf[256];
        snprintf(mbuf, 255, "%d", i);
        strs[i] = mbuf;
    }
    __block std::unordered_map<std::string, int> std_map;
    
    [self measureBlock:^{
        euint t = 0;
        for (int j = 0; j < 256; j++) {
            for (int i = 0; i < 1024; i++) {
                std_map.insert(std::make_pair(strs[i], i));
            }
            for (int i = 0; i < 1024; i++) {
                int v = std_map[strs[i]];
                t += v;
            }
            auto iter = std_map.begin();
            auto end = std_map.end();
            for (; iter != end; iter++) {
                t += iter->second;
            }
            std_map.clear();
        }
    }];
}

- (void) testCityHash
{
    [self measureBlock:^{
        euint64 value = 0;
        const char* s = "https://github.com/google/cityhash/blob/master/src/city.cchttps://github.com/google/cityhash/blob/master/src/city.cchttps://github.com/google/cityhash/blob/master/src/city.cc";
        euint32 len = (euint32)strlen(s);
        for (euint i = 0; i < 1024 * 1024; i++) {
            value += calc_cityhash32(s, len);
        }
    }];
}

- (void) testHashnr
{
    [self measureBlock:^{
        euint64 value = 0;
        const char* s = "https://github.com/google/cityhash/blob/master/src/city.cchttps://github.com/google/cityhash/blob/master/src/city.cchttps://github.com/google/cityhash/blob/master/src/city.cc";
        euint len = strlen(s);
        for (euint i = 0; i < 1024 * 1024; i++) {
            value += calc_hashnr(s, len);
        }
    }];
}

- (void) testGroup0
{
    xhn::group<int> group;
    for (int i = 0; i < 24; i++) {
        group.add(i);
    }
    printf("count %lld\n", group.size());
    auto iter = group.begin();
    auto end = group.end();
    euint iterCount = 0;
    for (; iter != end; iter++, iterCount++) {
        if (*iter == 12) {
            group.remove(iter);
            break;
        }
    }
    iterCount = 0;
    iter = group.begin();
    end = group.end();
    for (; iter != end; iter++, iterCount++) {
        printf("%d\n", *iter);
    }
    printf("here\n");
    XCTAssert(iterCount == 23, @"error");
}

- (void) testGroup1
{
    xhn::group<int> group;
    for (int i = 0; i < 24; i++) {
        group.add(i);
    }
    euint removeCount = 0;
    while (group.size()) {
        auto iter = group.begin();
        printf("remove:%d\n", *iter);
        group.remove(iter);
        removeCount++;
    }
    XCTAssert(removeCount == 24, @"error");
}

- (void) testAtomicOperator
{
    [self measureBlock:^{
        volatile esint64 a = 3;
        esint64 sum = 0;
        for (euint i = 0; i < 1024 * 1024 * 32; i++) {
            sum += AtomicLoad64(&a);
        }
    }];
}

- (void) testStringRFind
{
    {
        const char* key = "unavailable_icon.png";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.rfind(key);
        size_t pos1 = s1.rfind(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = ".png";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.rfind(key);
        size_t pos1 = s1.rfind(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "vaila";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.rfind(key);
        size_t pos1 = s1.rfind(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "aabbcc";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.rfind(key);
        size_t pos1 = s1.rfind(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "una";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.rfind(key);
        size_t pos1 = s1.rfind(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
}

- (void) testStringFind
{
    {
        const char* key = "unavailable_icon.png";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.find(key);
        size_t pos1 = s1.find(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = ".png";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.find(key);
        size_t pos1 = s1.find(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "vaila";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.find(key);
        size_t pos1 = s1.find(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "aabbcc";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.find(key);
        size_t pos1 = s1.find(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
    {
        const char* key = "una";
        xhn::string s0("unavailable_icon.png");
        std::string s1("unavailable_icon.png");
        euint pos0 = s0.find(key);
        size_t pos1 = s1.find(key);
        NSLog(@"pos0 = %lld, pos1 = %ld", pos0, pos1);
        XCTAssert(pos0 == pos1, @"error");
    }
}

- (void) testParallel0
{
    xhn::parallel p(3);
    volatile esint32 count = 0;
    p.parallel_for(0, 128, [&count](euint start, euint end){
        esint32 c = (esint32)(end - start);
        printf("start %lld, end %lld, c %d\n", start, end, c);
        esint32 s = 0;
        esint32 n = 0;
        do {
            s = AtomicLoad32(&count);
            n = s + c;
        }
        while (!AtomicCompareExchange(s, n, &count));
    });
    printf("count %d\n", count);
}

- (void) testParallel1
{
    ///xhn::parallel* p = VNEW xhn::parallel(3);
    [self measureBlock:^{
        volatile esint64 sum = 0;
        xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
        xhn::string b(" OPQ ");
        auto func =
        [&sum, a, b](euint start, euint end){
            for (euint i = start; i < end; i++) {
                sum += a.find(b);
            }
        };
        func(0, 33119);
        printf("sum:%lld\n", sum);
        XCTAssert(sum == 496785, @"error");
    }];
    ///VDELETE p;
}

- (void) testParallel2
{
    xhn::parallel* p = VNEW xhn::parallel(4);
    [self measureBlock:^{
        volatile esint64 sum = 0;
        xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
        xhn::string b(" OPQ ");
        auto func =
        [&sum, a, b](euint start, euint end){
            for (euint i = start; i < end; i++) {
                euint pos = a.find(b);
                esint64 s = 0;
                esint64 n = 0;
                do {
                    s = AtomicLoad64(&sum);
                    n = s + (esint64)pos;
                } while(!AtomicCompareExchange(s, n, &sum));
            }
        };
        p->parallel_for(0, 33119, func);
        ///printf("sum:%d\n", sum);
        XCTAssert(sum == 496785, @"error");
    }];
    VDELETE p;
}

- (void) testParallel3
{
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
        func(0, 1024 * 1024);
    }];
}

- (void) testParallel4
{
    xhn::parallel* p = VNEW xhn::parallel(3);
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
    }];
    VDELETE p;
}

- (void) testParallel5
{
    xhn::parallel* p = VNEW xhn::parallel(7);
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
    }];
    VDELETE p;
}

- (void) testParallel6
{
    xhn::parallel* p = VNEW xhn::parallel(3);
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
    }];
    VDELETE p;
}

- (void) testParallel7
{
    xhn::parallel* p = VNEW xhn::parallel(7);
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for(0, 1024 * 1024, func);
    }];
    VDELETE p;
}

- (void) testParallel8
{
    xhn::parallel* p = VNEW xhn::parallel(7);
    [self measureBlock:^{
        auto func =
        [](euint start, euint end){
            xhn::string a("ABC DEF HIJ XMN OPQ RST UVW XYZ");
            xhn::string b(" OPQ ");
            for (euint i = start; i < end; i++) {
                a.find(b);
            }
        };
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
        p->parallel_for_async(0, 1024 * 1024, func);
    }];
    VDELETE p;
}

class TestObject : public RefObject
{
};

- (void) testSharedPtr
{
    [self measureBlock:^{
        std::weak_ptr<TestObject> w;
        {
            std::shared_ptr<TestObject> a(VNEW TestObject);
            w = a;
            for (int _ = 0; _ < 1024 * 1024; _++) {
                std::shared_ptr<TestObject> b(a);
                std::shared_ptr<TestObject> c(a);
                std::shared_ptr<TestObject> d(a);
                std::shared_ptr<TestObject> e(a);
                
                std::shared_ptr<TestObject> f(a);
                std::shared_ptr<TestObject> i(a);
                std::shared_ptr<TestObject> j(a);
                std::shared_ptr<TestObject> k(a);
                
                std::shared_ptr<TestObject> l(a);
                std::shared_ptr<TestObject> m(a);
                std::shared_ptr<TestObject> n(a);
                std::shared_ptr<TestObject> o(a);
                
                std::shared_ptr<TestObject> p(a);
                std::shared_ptr<TestObject> q(a);
                std::shared_ptr<TestObject> r(a);
                std::shared_ptr<TestObject> s(a);
                /**
                /// 这样写直接崩溃
                TestObject* ptr = c.get();
                std::shared_ptr<TestObject> z(ptr);
                **/
            }
        }
        std::shared_ptr<TestObject> s = w.lock();
        XCTAssert(!s.get(), @"error");
    }];
}

- (void) testSmartPtr
{
    [self measureBlock:^{
        xhn::WeakPtr<TestObject> w;
        {
            xhn::SmartPtr<TestObject> a(VNEW TestObject);
            a.ToWeakPtr(w);
            for (int _ = 0; _ < 1024 * 1024; _++) {
                xhn::SmartPtr<TestObject> b(a);
                xhn::SmartPtr<TestObject> c(a);
                xhn::SmartPtr<TestObject> d(a);
                xhn::SmartPtr<TestObject> e(a);
                
                xhn::SmartPtr<TestObject> f(a);
                xhn::SmartPtr<TestObject> i(a);
                xhn::SmartPtr<TestObject> j(a);
                xhn::SmartPtr<TestObject> k(a);
                
                xhn::SmartPtr<TestObject> l(a);
                xhn::SmartPtr<TestObject> m(a);
                xhn::SmartPtr<TestObject> n(a);
                xhn::SmartPtr<TestObject> o(a);
                
                xhn::SmartPtr<TestObject> p(a);
                xhn::SmartPtr<TestObject> q(a);
                xhn::SmartPtr<TestObject> r(a);
                xhn::SmartPtr<TestObject> s(a);
            }
        }
        xhn::SmartPtr<TestObject> s = w.ToStrongPtr();
        XCTAssert(!s.get(), @"error");
    }];
}

- (void) testNthElement0
{
    int a[256];
    for (int i = 0; i < 256; i++) {
        a[i] = rand();
    }
    int x = a[128];
    xhn::nth_element_impl<int*, int>(a, a + 128, a + 256);
    for (int i = 0; i < 256; i++) {
        if (a[i] < x)
            printf("%012d < %012d\n", a[i], x);
        else if (a[i] > x)
            printf("%012d > %012d\n", a[i], x);
        else
            printf("%012d == %012d\n", a[i], x);
    }
    printf("here\n");
}

- (void) testNthElement1
{
    std::vector<int> a;
    for (int i = 0; i < 256; i++) {
        a.push_back(rand());
    }
    int x = a[128];
    auto first = a.begin();
    auto nth = a.begin() + 128;
    auto last = a.end();
    std::nth_element(first, nth,last);
    for (int i = 0; i < 256; i++) {
        if (a[i] < x)
            printf("%012d < %012d\n", a[i], x);
        else if (a[i] > x)
            printf("%012d > %012d\n", a[i], x);
        else
            printf("%012d == %012d\n", a[i], x);
    }
    printf("::::%012d\n", *nth);
    printf("here\n");
}

- (void) testNthElement2
{
    [self measureBlock:^{
        int a[1024];
        for (int i = 0; i < 1024; i++) {
            a[i] = rand();
        }
        for (int i = 0; i < 128 * 1024; i++) {
            xhn::nth_element<int*>(a, a + 512, a + 1024);
        }
    }];
}

- (void) testNthElement3
{
    [self measureBlock:^{
        int a[1024];
        for (int i = 0; i < 1024; i++) {
            a[i] = rand();
        }
        for (int i = 0; i < 128 * 1024; i++) {
            std::nth_element(a, a + 512, a + 1024);
        }
    }];
}

@end
