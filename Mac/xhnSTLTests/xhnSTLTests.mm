

#import <XCTest/XCTest.h>
#include <xhnSTL/xhn_thread.hpp>
#include <xhnSTL/xhn_lambda.hpp>
#include <xhnSTL/timer.h>
#include <xhnSTL/xhn_atomic_ptr.hpp>
#include <xhnSTL/xhn_smart_ptr.hpp>
#include <xhnSTL/xhn_vector.hpp>
#include <xhnSTL/xhn_group.hpp>
#include <xhnSTL/xhn_parallel.hpp>
#include <xhnSTL/xhn_concurrent.hpp>
#include <xhnSTL/xhn_stacktrace.hpp>
#include <xhnSTL/xhn_realtime_thread.hpp>
#include <xhnSTL/xhn_open_hash_set.hpp>
#include <xhnSTL/xhn_math.hpp>
#include <xhnSTL/xhn_neural_network.hpp>
#include <xhnSTL/xhn_logger.hpp>
#include <xhnSTL/cpu.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cxxabi.h>

#define STD xhn
#include "ColladaTool.hpp"


@interface xhnSTLTests : XCTestCase

@end

@implementation xhnSTLTests

- (void)setUp {
    [super setUp];
    //download_http("http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz");
    //download_http("http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz");
}

- (void)tearDown {
    [super tearDown];
}

- (void) testUrl
{
    xhn::string path = "/Users/xhnsworks/VEngineProjects/未命名项目/COLLADATextures/Archer/Female/Archer_Female_dds/Archer_Female_01.dds";
    xhn::string url = xhn::to_url(path);
    printf("URL:%s\n", url.c_str());
    xhn::string newpath = xhn::to_utf8_path(url);
    printf("NEWPATH:%s\n", newpath.c_str());
    XCTAssert(path == newpath, @"error");
}

- (void) testXhnSort
{
    xhn::vector<int> array;
    array.push_back(3);
    array.push_back(0);
    array.push_back(1);
    xhn::sort(array.begin(), array.end());
    XCTAssert(array.size() == 3, @"error");
    XCTAssert(array[0] == 0, @"error");
    XCTAssert(array[1] == 1, @"error");
    XCTAssert(array[2] == 3, @"error");
    
    array.push_back(128);
    array.push_back(23);
    array.push_back(-88);
    array.push_back(512);
    array.push_back(-9999);
    xhn::sort(array.begin(), array.end());
    for (auto i : array) {
        printf("%d\n", i);
    }
}

- (void) testXhnString
{
    std::string stdstr =
    "abcdefghi\n"
    "defghiabc\n"
    "abdghidef\n"
    "ghidefdef\n";
    xhn::string xhnstr =
    "abcdefghi\n"
    "defghiabc\n"
    "abdghidef\n"
    "ghidefdef\n";
    euint xhnpos = xhnstr.find("def", 13);
    size_t stdpos = stdstr.find("def", 13);
    printf("stnpos=%lld, stdpos=%ld\n", xhnpos, stdpos);
    XCTAssert(xhnpos == stdpos, @"error");
}

- (void) testColladaTool
{
    VEngine::ColladaTool tool;
    tool.m_colladaContent =
    "abcdefghi\n"
    "defghiabc\n"
    "abdghidef\n"
    "ghidefdef\n";
    xhn::string correctResult =
    "abc-A-ghi\n"
    "-A-ghiabc\n"
    "abdghi-A-\n"
    "ghi-A--A-\n";
    xhn::vector<xhn::pair<xhn::string, xhn::string>> resetDirVec;
    resetDirVec.push_back(xhn::make_pair(xhn::string("def"), xhn::string("-A-")));
    tool.ResetTexturesDirectory(resetDirVec);
    printf("%s\n", tool.m_colladaContent.c_str());
    XCTAssert(tool.m_colladaContent == correctResult, @"error");
}

- (void) testAtomicPtr {
    int a = 0;
    xhn::AtomicPtr<int> b(&a);
    *b = 32;
    XCTAssert(a == 32, "a != 32");
}

class Test : public RefObjectBase, public MemObject
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
    xhn::vector<int> intArray( {0, 1, 2, 3, 4, 5} );
    xhn::Lambda<void(int&, bool*)> proc([](int& i, bool* stop){
        printf("%d\n", i);
        *stop = true;
    });
    intArray.for_each(proc);
    
    intArray.throw_front(intArray.begin() + 2);
    
    for (auto i : intArray) {
        printf("for %d\n", i);
    }
    
    auto iter = intArray.begin();
    iter += 3;
    euint offs = iter - intArray.begin();
    printf("offs %lld\n", offs);
}

- (void) testVector2 {
    xhn::vector< xhn::vector<int>,
                 xhn::FGetElementRealSizeProc<xhn::vector<int>>,
                 xhn::FVectorNextProc<xhn::vector<int>>,
                 xhn::FVectorPrevProc<xhn::vector<int>> > intArray;
    intArray.push_back( xhn::vector<int>() );
    intArray.push_back( xhn::vector<int>() );
    xhn::Lambda<void(xhn::vector<int>&, bool*)> proc([](xhn::vector<int>& i, bool* stop){
        *stop = true;
    });
    intArray.for_each(proc);
}

- (void) testVector3 {
    xhn::vector<int> ia;
    std::vector<int> sia;
    ia.push_back(0);
    ia.push_back(1);
    ia.push_back(2);
    ia.push_back(3);
    ia.push_back(4);
    ia.push_back(5);
    sia.push_back(0);
    sia.push_back(1);
    sia.push_back(2);
    sia.push_back(3);
    sia.push_back(4);
    sia.push_back(5);
    {
        auto iter = ia.rbegin();
        auto end = ia.rend();
        auto siter = sia.rbegin();
        ///auto send = sia.rend();
        for (; iter != end; iter++, siter++) {
            XCTAssert(*iter == *siter, @"error");
        }
    }
    {
        const auto& cia = ia;
        const auto& csia = sia;
        auto iter = cia.rbegin();
        auto end = cia.rend();
        auto siter = csia.rbegin();
        for (; iter != end; iter++, siter++) {
            XCTAssert(*iter == *siter, @"error");
        }
    }
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#1:%d\n", *v);
    }
}

- (void)testDictionary2
{
    xhn::dictionary<const char*, int> hash_table;
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#2:%d\n", *v);
    }
}

- (void)testDictionary3
{
    xhn::dictionary<xhn::static_string, int> hash_table;
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        hash_table.insert(mbuf, i);
    }
    
    for (int i = 0; i < 100; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
        int* v = hash_table.find(mbuf);
        printf("#2:%d\n", *v);
    }
}

- (void) testDictionary4
{
    char mbufs[100][256];
    for (int i = 0; i < 100; i++) {
        snprintf(&mbufs[i][0], 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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

- (void) testDictionary7
{
    xhn::dictionary<int, xhn::string> hash_table;
    hash_table.insert(0x0001, "abc");
    xhn::hash_node<int, xhn::string>* node0 = hash_table.find_hash_node(0x0001);
    printf("node0:%d\n", node0->m_count);
    hash_table.insert(0x1001, "def");
    xhn::hash_node<int, xhn::string>* node1 = hash_table.find_hash_node(0x1001);
    printf("node1:%d\n", node1->m_count);
    hash_table.remove(0x1001);
    printf("node0:%d\n", node0->m_count);
    XCTAssert(node0->m_count == 1, @"node0->m_count != 1");
}

- (void) testXhnMap1
{
    __block int num = 1024 * 128;
    __block xhn::static_string* strs = VNEW_ARRAY xhn::static_string[num];
    for (int i = 0; i < num; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        snprintf(mbuf, 256, "abc%dde%dfgh", i, i);
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
        int size = snprintf(mbuf, 256, "%d00000000000", i);
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
        int size = snprintf(mbuf, 256, "%d00000000000", i);
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
        int size = snprintf(mbuf, 256, "%dabcdefxyzwwww%d", i, i);
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
        int size = snprintf(mbuf, 256, "%dabcdefxyzwwww%d", i, i);
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
        snprintf(mbuf, 256, "%d", i);
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

- (void) testCachePerformance1P
{
    static xhn::static_string strs[1024];
    for (int i = 0; i < 1024; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "%d", i);
        strs[i] = mbuf;
    }
    __block xhn::cache<xhn::static_string, int, 1024>* testcache = VNEW xhn::cache<xhn::static_string, int, 1024>();
    for (int i = 0; i < 1024; i++) {
        testcache->insert(strs[i], i);
    }
    __block xhn::parallel* p = VNEW xhn::parallel(7);
    [self measureBlock:^{
        volatile esint64 result = 0;
        xhn::cache<xhn::static_string, int, 1024>* _t = testcache;
        auto proc = [_t, &result](euint begin, euint end) {
            euint t = 0;
            for (euint j = begin; j < end; j++) {
                for (int i = 0; i < 1024; i++) {
                    int* v = _t->find(strs[i]);
                    t += *v;
                }
            }
            esint64 r = AtomicLoad64(&result);
            while (!AtomicCompareExchange(r, r + t, &result)) {
                nanopause(1);
                r = AtomicLoad64(&result);
            }
        };
        p->parallel_for(0, 1024 * 16, proc);
        printf("P:result = %lld\n", result);
    }];
    VDELETE p;
    VDELETE testcache;
}

- (void) testCachePerformance1NP
{
    static xhn::static_string strs[1024];
    for (int i = 0; i < 1024; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "%d", i);
        strs[i] = mbuf;
    }
    __block xhn::cache<xhn::static_string, int, 1024>* testcache = VNEW xhn::cache<xhn::static_string, int, 1024>();
    for (int i = 0; i < 1024; i++) {
        testcache->insert(strs[i], i);
    }
    __block xhn::parallel* p = VNEW xhn::parallel(7);
    [self measureBlock:^{
        euint t = 0;
        xhn::cache<xhn::static_string, int, 1024>* _t = testcache;
        auto proc = [&t, _t](euint begin, euint end) {
            for (euint j = begin; j < end; j++) {
                for (int i = 0; i < 1024; i++) {
                    int* v = _t->find(strs[i]);
                    t += *v;
                }
            }
        };
        proc(0, 1024 * 16);
        printf("NP:result = %lld\n", t);
    }];
    VDELETE p;
    VDELETE testcache;
}

- (void) testStdUnorderedMapPerformance0
{
    static std::string strs[1024];
    for (int i = 0; i < 1024; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "%d", i);
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

class TestObject : public RefObjectBase, public MemObject
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
            xhn::nth_element_impl<int*, int>(a, a + 512, a + 1024);
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

struct FLessThanProcInt
{
    bool operator () (int a, int b) const {
        return a < b;
    }
};
- (void) testFilter0
{
    [self measureBlock:^{
        int min = INT_MAX;
        int max = -INT_MAX;
        int mid = 0;
        int a[1024];
        for (int i = 0; i < 1024; i++) {
            a[i] = rand();
            if (a[i] > max) {
                max = a[i];
            }
            if (a[i] < min) {
                min = a[i];
            }
        }
        mid = (max - min) / 2;
        for (int i = 0; i < 128 * 1024; i++) {
            xhn::filter<int*, int, FLessThanProcInt>(a, a + 1024, mid);
        }
    }];
}

- (void) testCachePerformance2
{
    static xhn::static_string* testStrs = (xhn::static_string*)NMalloc(sizeof(xhn::static_string) * 1024 * 128);
    for (int i = 0; i < 1024 * 128; i++) {
        char mbuf[256];
        snprintf(mbuf, 256, "%d", i);
        testStrs[i] = mbuf;
    }
    __block xhn::cache<xhn::static_string, int, 1024 * 128>* testcache =
    VNEW xhn::cache<xhn::static_string, int, 1024 * 128>();
    for (int i = 0; i < 1024 * 128; i++) {
        testcache->insert(testStrs[i], i);
    }
    testcache->clear();
    [self measureBlock:^{
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        VTime t;
        for (int i = 0; i < 1024 * 128; i++) {
            testcache->insert(testStrs[i], i);
        }
        TimeCheckpoint::Tock(tp, t);
        printf("INSERT TIME:%f US\n", t.GetMicrosecond());
        tp = TimeCheckpoint::Tick();
        testcache->clear();
        TimeCheckpoint::Tock(tp, t);
        printf("CLEAR TIME:%f US\n", t.GetMicrosecond());
    }];
    VDELETE testcache;
    Mfree(testStrs);
}

- (void) testCalcLimit0
{
    __block xhn::vector<float>* t = VNEW xhn::vector<float>();
    for (euint i = 0; i < 1024 * 1024; i++) {
        t->push_back(float(rand() - rand()));
    }
    [self measureBlock:^{
        float min, max, mid;
        euint mid_index;
        
        xhn::calc_limit<xhn::vector<float>, float, euint,
        xhn::FInitMaxFloat, xhn::FInitMinFloat, xhn::FInitMidIndexEUint,
        xhn::FAbsFloat, xhn::FCalcMidFloat,
        xhn::FGreaterThanSetFloat, xhn::FLessThanSetFloat,
        xhn::FLessProcEUint, xhn::FIncProcEUint,
        xhn::FLessThanSet2FloatEUint>(*t, min, max, mid, mid_index, 0, t->size());
        
        printf("min:%f, max:%f, mid:%f, mid_index:%lld, %f\n", min, max, mid, mid_index, (*t)[mid_index]);
    }];
    VDELETE t;
}

- (void) testFilter1
{
    int a0[] = {6, 8, 3, 7, 5, 1, 4, 2, 9};
    int a1[] = {8, 2, 4, 3, 9, 6, 7, 1, 5};
    int a2[] = {2, 1, 4, 3, 6, 5, 9, 8, 7};
    int* a0_ = xhn::filter<int*, int, FLessThanProcInt>(a0, a0 + 9, 5);
    int* a1_ = xhn::filter<int*, int, FLessThanProcInt>(a1, a1 + 9, 5);
    int* a2_ = xhn::filter<int*, int, FLessThanProcInt>(a2, a2 + 9, 5);
    
    for (int* i = a0; i < a0_; i++) {
        XCTAssert(*i < 5, @"error");
    }
    for (int* i = a0_; i < a0 + 9; i++) {
        XCTAssert(*i >= 5, @"error");
    }
    for (int* i = a1; i < a1_; i++) {
        XCTAssert(*i < 5, @"error");
    }
    for (int* i = a1_; i < a1 + 9; i++) {
        XCTAssert(*i >= 5, @"error");
    }
    for (int* i = a2; i < a2_; i++) {
        XCTAssert(*i < 5, @"error");
    }
    for (int* i = a2_; i < a2 + 9; i++) {
        XCTAssert(*i >= 5, @"error");
    }
}

class ThreadLocalVector : public xhn::concurrent_variable
{
    DeclareRTTI;
public:
    xhn::vector<int> m_vector;
};

ImplementRTTI(ThreadLocalVector, xhn::concurrent_variable);

- (void) testConcurrent
{
    xhn::concurrent concurrent(8);
    
    xhn::Lambda<void (xhn::thread_local_variables_t&)>
    createVariables([](xhn::thread_local_variables_t& variables){
        variables.insert("ThreadLocalVector", VNEW ThreadLocalVector);
    });
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    concurrent.execute(createVariables);
    xhn::thread::micro_sleep(100);
    xhn::Lambda<void (xhn::thread_local_variables_t&)>
    testVariables([](xhn::thread_local_variables_t& variables){
        xhn::concurrent_variable_ptr* tlv = variables.find("ThreadLocalVector");
        if (tlv) {
            printf("ThreadLocalVector existed!\n");
        }
        else {
            printf("ThreadLocalVector not existed???\n");
        }
    });
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    concurrent.execute(testVariables);
    
    concurrent.waiting_for_completed();
}

- (void) testVectorErase
{
    xhn::vector<int> vec;
    for (int i = 0; i < 16; i++) {
        vec.push_back(i);
    }
    auto iter = vec.begin() + 5;
    XCTAssert(*iter == 5, @"error");
    vec.erase(iter);
    XCTAssert(vec.size() == 15, @"error");
    iter = vec.begin();
    auto end = vec.end();
    for (; iter != end; iter++) {
        printf("%d\n", *iter);
    }
}

- (void) testVectorRemove
{
    xhn::vector<int> vec;
    for (int i = 0; i < 16; i++) {
        vec.push_back(i);
    }
    auto iter = vec.begin() + 5;
    XCTAssert(*iter == 5, @"error");
    vec.remove(iter);
    XCTAssert(vec.size() == 15, @"error");
    iter = vec.begin();
    auto end = vec.end();
    for (; iter != end; iter++) {
        printf("%d\n", *iter);
    }
}

- (void) testVectorFor
{
    xhn::vector<int> vec;
    std::vector<int> svec;
    for (int i = 0; i < 16; i++) {
        vec.push_back(i);
        svec.push_back(i);
    }
    {
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        int sum = 0;
        auto iter = vec.begin();
        auto end = vec.end();
        for (; iter != end; iter++) {
            sum += *iter;
        }
        VTime t;
        TimeCheckpoint::Tock(tp, t);
        printf("xhn for time:%f\n", t.GetNanosecond());
    }
    {
        TimeCheckpoint tp = TimeCheckpoint::Tick();
        int sum = 0;
        auto iter = svec.begin();
        auto end = svec.end();
        for (; iter != end; iter++) {
            sum += *iter;
        }
        VTime t;
        TimeCheckpoint::Tock(tp, t);
        printf("std for time:%f\n", t.GetNanosecond());
    }
}

- (void) testVectorInsert
{
    xhn::vector<int> vec;
    std::vector<int> svec;
    for (int i = 0; i < 16; i++) {
        vec.push_back(i);
        svec.push_back(i);
    }
    {
        auto iter = vec.begin() + 5;
        vec.insert(iter, 100);
    }
    {
        auto iter = svec.begin() + 5;
        svec.insert(iter, 100);
    }
    {
        XCTAssert(vec.size() == svec.size(), @"error");
        for (euint i = 0; i < vec.size(); i++) {
            XCTAssert(vec[i] == svec[i], @"error: %d != %d", vec[i], svec[i]);
        }
    }
}
- (void) testVectorRangeInsert
{
    xhn::vector<int> vec;
    xhn::vector<int> rv;
    std::vector<int> svec;
    std::vector<int> srv;
    rv.push_back(100);
    rv.push_back(200);
    rv.push_back(300);
    srv.push_back(100);
    srv.push_back(200);
    srv.push_back(300);
    for (int i = 0; i < 16; i++) {
        vec.push_back(i);
        svec.push_back(i);
    }
    {
        auto iter = vec.begin() + 5;
        vec.insert(iter, rv.begin(), rv.end());
    }
    {
        auto iter = svec.begin() + 5;
        svec.insert(iter, srv.begin(), srv.end());
    }
    {
        XCTAssert(vec.size() == svec.size(), @"error");
        for (euint i = 0; i < vec.size(); i++) {
            XCTAssert(vec[i] == svec[i], @"error: %d != %d", vec[i], svec[i]);
        }
    }
}

- (void) testVectorPerformance0
{
    [self measureBlock:^{
        xhn::vector<euint> vec;
        vec.reserve(1024 * 1024);
        for (euint i = 0; i < 1024 * 1024; i++) {
            vec.push_back(i);
        }
    }];
}

- (void) testVectorPerformance1
{
    [self measureBlock:^{
        std::vector<euint> vec;
        vec.reserve(1024 * 1024);
        for (euint i = 0; i < 1024 * 1024; i++) {
            vec.push_back(i);
        }
    }];
}

- (void) testVectorPerformance2
{
    __block xhn::vector<euint>* vec = VNEW xhn::vector<euint>();
    vec->reserve(1024 * 16);
    for (euint i = 0; i < 1024 * 16; i++) {
        vec->push_back(i);
    }
    [self measureBlock:^{
        euint i = 0;
        while (!vec->empty()) {
            auto iter = vec->begin();
            auto end = vec->end();
            for (; iter != end; iter++) {
                if (*iter == i) {
                    vec->erase(iter);
                    i++;
                    break;
                }
            }
        }
    }];
    VDELETE vec;
}

- (void) testVectorPerformance3
{
    __block std::vector<euint>* vec = new std::vector<euint>();
    vec->reserve(1024 * 16);
    for (euint i = 0; i < 1024 * 16; i++) {
        vec->push_back(i);
    }
    [self measureBlock:^{
        euint i = 0;
        while (!vec->empty()) {
            auto iter = vec->begin();
            auto end = vec->end();
            for (; iter != end; iter++) {
                if (*iter == i) {
                    vec->erase(iter);
                    i++;
                    break;
                }
            }
        }
    }];
    delete vec;
}

/// 0 1 2 3 4 5 6 7 8 9
/// E X E E X E X E E E
struct IntProc0
{
    xhn::vector<int>::for_each_status operator() (int i) {
        if (1 == i || 4 == i || 6 == i) {
            return xhn::vector<int>::RemoveAndContinue;
        }
        else {
            return xhn::vector<int>::KeepAndContinue;
        }
    }
};

/// 0 1 2 3 4 5 6 7 8 9
/// E X E E X E X X E E
struct IntProc1
{
    xhn::vector<int>::for_each_status operator() (int i) {
        if (1 == i || 4 == i || 6 == i || 7 == i) {
            return xhn::vector<int>::RemoveAndContinue;
        }
        else {
            return xhn::vector<int>::KeepAndContinue;
        }
    }
};

/// 0 1 2 3 4 5 6 7 8 9
/// X X E E E E E E E E
struct IntProc2
{
    xhn::vector<int>::for_each_status operator() (int i) {
        if (0 == i || 1 == i) {
            return xhn::vector<int>::RemoveAndContinue;
        }
        else {
            return xhn::vector<int>::KeepAndContinue;
        }
    }
};

/// 0 1 2 3 4 5 6 7 8 9
/// E E X X X E E E E E

struct IntProc3
{
    xhn::vector<int>::for_each_status operator() (int i) {
        if (i >= 2 && i <= 4) {
            return xhn::vector<int>::RemoveAndContinue;
        }
        else {
            return xhn::vector<int>::KeepAndContinue;
        }
    }
};

/// 0 1 2 3 4 5 6 7 8 9
/// E E E E E E E E X X
struct IntProc4
{
    xhn::vector<int>::for_each_status operator() (int i) {
        if (i >= 8) {
            return xhn::vector<int>::RemoveAndContinue;
        }
        else {
            return xhn::vector<int>::KeepAndContinue;
        }
    }
};
- (void) testVectorForEach
{
    {
        /// 0 1 2 3 4 5 6 7 8 9
        /// E X E E X E X E E E
        xhn::vector<int> testVec;
        for (int i = 0; i < 10; i++) {
            testVec.push_back(i);
        }
        XCTAssert(testVec.size() == 10, @"error");
        IntProc0 proc;
        testVec.for_each(proc);
        for (auto i : testVec) {
            printf("%d\n", i);
            XCTAssert((1 != i && 4 != i && 6 != i), @"error");
        }
        printf("size == %lld\n", testVec.size());
        XCTAssert(testVec.size() == 7, @"error");
        printf("here\n");
    }
    {
        /// 0 1 2 3 4 5 6 7 8 9
        /// E X E E X E X X E E
        xhn::vector<int> testVec;
        for (int i = 0; i < 10; i++) {
            testVec.push_back(i);
        }
        XCTAssert(testVec.size() == 10, @"error");
        IntProc1 proc;
        testVec.for_each(proc);
        for (auto i : testVec) {
            printf("%d\n", i);
            XCTAssert((1 != i && 4 != i && 6 != i && 7 != i), @"error");
        }
        printf("size == %lld\n", testVec.size());
        XCTAssert(testVec.size() == 6, @"error");
        printf("here\n");
    }
    {
        /// 0 1 2 3 4 5 6 7 8 9
        /// X X E E E E E E E E
        xhn::vector<int> testVec;
        for (int i = 0; i < 10; i++) {
            testVec.push_back(i);
        }
        XCTAssert(testVec.size() == 10, @"error");
        IntProc2 proc;
        testVec.for_each(proc);
        for (auto i : testVec) {
            printf("%d\n", i);
            XCTAssert((0 != i && 1 != i), @"error");
        }
        printf("size == %lld\n", testVec.size());
        XCTAssert(testVec.size() == 8, @"error");
        printf("here\n");
    }
    {
        /// 0 1 2 3 4 5 6 7 8 9
        /// E E X X X E E E E E
        xhn::vector<int> testVec;
        for (int i = 0; i < 10; i++) {
            testVec.push_back(i);
        }
        XCTAssert(testVec.size() == 10, @"error");
        IntProc3 proc;
        testVec.for_each(proc);
        for (auto i : testVec) {
            printf("%d\n", i);
            XCTAssert((i < 2 || i > 4), @"error");
        }
        printf("size == %lld\n", testVec.size());
        XCTAssert(testVec.size() == 7, @"error");
        printf("here\n");
    }
    {
        /// 0 1 2 3 4 5 6 7 8 9
        /// E E E E E E E E X X
        xhn::vector<int> testVec;
        for (int i = 0; i < 10; i++) {
            testVec.push_back(i);
        }
        XCTAssert(testVec.size() == 10, @"error");
        IntProc4 proc;
        testVec.for_each(proc);
        for (auto i : testVec) {
            printf("%d\n", i);
            XCTAssert((i < 8), @"error");
        }
        printf("size == %lld\n", testVec.size());
        XCTAssert(testVec.size() == 8, @"error");
        printf("here\n");
    }
}



- (void) testStacktrace
{
    xhn::test_stacktrace();
}

- (void) testMutexLock
{
    [self measureBlock:^{
        xhn::MutexLock lock;
        for (euint i = 0; i < 10000000; i++) {
            auto inst = lock.Lock();
        }
    }];
}

- (void) testSpinLock
{
    [self measureBlock:^{
        xhn::SpinLock lock;
        for (euint i = 0; i < 10000000; i++) {
            auto inst = lock.Lock();
        }
    }];
}
    
static int s_exceCount = 0;
    
class MyRealtimeTask : public xhn::realtime_thread::realtime_task
{
public:
    virtual const xhn::static_string type() const { return ""; }
    virtual ~MyRealtimeTask() {}
    virtual void run_once(double elapsedTime) {
        printf("run once %d, %f\n", s_exceCount, elapsedTime);
        s_exceCount++;
    }
};
    
- (void) testRealtimeThread
{
    xhn::realtime_thread::realtime_task_pool_ptr taskPool;
    taskPool = VNEW xhn::realtime_thread::realtime_task_pool();
    {
        auto tasks = taskPool->GetTasks().Lock();
        tasks->push_back(VNEW MyRealtimeTask());
    }
    
    {
        xhn::realtime_thread thread(taskPool, 0.016666);
        thread.run();
        while (s_exceCount < 600) {
            xhn::thread::nano_sleep(1000000);
        }
    }
}

- (void) testOpenHashSet
{
    xhn::open_hash_set<TestPtr> testSet;
    TestPtr a = VNEW Test();
    TestPtr b = VNEW Test();
    a->a = 32;
    b->a = 128;
    testSet.insert(a);
    testSet.insert(a);
    testSet.insert(b);
    testSet.insert(b);
    auto iter = testSet.begin();
    auto end = testSet.end();
    for (; iter != end; iter++) {
        TestPtr t = *iter;
        printf("%d\n", t->a);
    }
    printf("here\n");
}

double legendre0(double x)
{
    return 1.0;
}
double legendre1(double x)
{
    return x;
}
double legendre2(double x)
{
    return 1.0 / 2.0 * (3.0 * x * x -
                        1.0);
}
double legendre3(double x)
{
    return 1.0 / 2.0 * (5.0 * x * x * x -
                        3.0 * x);
}
double legendre4(double x)
{
    return 1.0 / 8.0 * (35.0 * x * x * x * x -
                        30.0 * x * x +
                        3.0);
}
- (void) testLegendre
{
    double a[5] = {-1.0, -0.5, 0.0, 0.5, 1.0};
    for (int i = 0; i < 5; i++) {
        XCTAssert(xhn::float_equal(xhn::legendre(0, a[i]), legendre0(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::legendre(1, a[i]), legendre1(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::legendre(2, a[i]), legendre2(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::legendre(3, a[i]), legendre3(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::legendre(4, a[i]), legendre4(a[i]), 0.01), @"error");
    }
}

double assoc_legendre00(double x)
{
    return 1.0;
}
double assoc_legendre10(double x)
{
    return x;
}
double assoc_legendre11(double x)
{
    return -pow(1.0 - x * x, 0.5);
}
double assoc_legendre20(double x)
{
    return 1.0 / 2.0 * (3.0 * x * x-
                        1.0);
}
double assoc_legendre21(double x)
{
    return -3.0 * x * pow(1.0 - x * x, 0.5);
}
double assoc_legendre22(double x)
{
    return 3.0 * (1.0 - x * x);
}
- (void) testAssocLegendre
{
    double a[5] = {-1.0, -0.5, 0.0, 0.5, 1.0};
    for (int i = 0; i < 5; i++) {
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(0, 0, a[i]), assoc_legendre00(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(1, 0, a[i]), assoc_legendre10(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(1, 1, a[i]), assoc_legendre11(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(2, 0, a[i]), assoc_legendre20(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(2, 1, a[i]), assoc_legendre21(a[i]), 0.01), @"error");
        XCTAssert(xhn::float_equal(xhn::assoc_legendre(2, 2, a[i]), assoc_legendre22(a[i]), 0.01), @"error");
    }
}
- (void) testFactorial
{
    XCTAssert(xhn::factorial(10) == 3628800, @"error");
    XCTAssert(xhn::factorial(15) == 1307674368000, @"error");
    XCTAssert(xhn::factorial(20) == 2432902008176640000, @"error");
    char mbuf[128];
    euint128 v = xhn::factorial128(34);
    xhn::u128toa(v, mbuf);
    printf("%s\n", mbuf);
    v = static_cast<euint128>(-1);
    xhn::u128toa(v, mbuf);
    printf("%s\n", mbuf);
}
- (void) testLogger
{
    xhn::logger<xhn::CLoggerImpl> logger;
    logger.log("a(%f), b(%s), c(%d)\n", 0.15, "aa", 12);
}

- (void) testSigmoid
{
    float x = 0.5f;
    float current = xhn::sigmoid(x);
    float target = 0.5f;
    printf("current = %f\n", current);
    float d = xhn::derivative_sigmoid(current);
    printf("d = %f\n", d);
    float delta = target - current;
    float offset = delta / d;
    x += offset;
    current = xhn::sigmoid(x);
    printf("current = %f\n", current);
    target = 0.8f;
    delta = target - current;
    offset = delta / d;
    x += offset;
    current = xhn::sigmoid(x);
    printf("current = %f\n", current);
}

double image0[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.44,0.93,1.00,0.78,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.55,0.99,0.78,0.87,0.93,0.69,0.67,0.10,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.84,1.00,0.61,0.03,0.52,0.91,0.60,0.96,0.75,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.74,0.96,0.61,0.02,0.00,0.25,0.18,0.00,0.07,0.95,0.25,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.71,0.93,0.25,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.61,0.26,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.53,0.93,0.26,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.55,0.17,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.53,0.96,0.24,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.04,0.79,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.49,0.99,0.61,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.82,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.76,0.74,0.02,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.48,0.69,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.65,0.90,0.18,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.85,0.39,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.96,0.35,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.92,0.34,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.91,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.80,0.65,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.75,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.44,0.86,0.99,0.16,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.25,0.99,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.24,0.96,0.88,0.33,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.99,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.52,0.93,0.98,0.42,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.99,0.04,0.00,0.00,0.00,0.00,0.00,0.15,0.81,0.99,0.86,0.40,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.99,0.04,0.00,0.00,0.04,0.05,0.37,0.89,0.99,0.65,0.13,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.99,0.62,0.61,0.61,0.96,0.99,0.99,0.92,0.53,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.99,0.99,0.99,0.99,0.99,0.94,0.37,0.15,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.04,0.13,0.77,0.99,0.65,0.54,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image1[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.39,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.83,0.99,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.65,0.99,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.29,0.98,0.99,0.06,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.19,0.96,0.99,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.92,1.00,0.77,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.59,0.99,0.87,0.07,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.27,0.99,0.92,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.16,0.99,0.89,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.16,0.99,1.00,0.15,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.94,1.00,0.15,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.59,1.00,0.48,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.38,1.00,0.65,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.33,1.00,0.91,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.91,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,1.00,0.31,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.99,0.31,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.99,0.31,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.93,0.99,0.31,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.32,0.93,0.07,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image2[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.81,0.99,1.00,0.81,0.35,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.30,0.84,0.99,0.99,0.99,0.99,0.96,0.48,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.88,0.94,0.71,0.22,0.47,0.96,0.99,0.90,0.10,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.88,0.76,0.00,0.00,0.00,0.48,0.96,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.88,0.76,0.00,0.00,0.00,0.00,0.77,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.49,0.12,0.00,0.00,0.00,0.00,0.78,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.28,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.77,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.77,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.33,0.99,0.88,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.33,0.33,0.15,0.00,0.82,0.99,0.39,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.13,0.47,0.84,0.99,0.99,0.87,0.78,0.96,0.82,0.07,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.55,0.99,0.95,0.87,0.89,0.99,0.99,0.99,0.76,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.80,0.99,0.35,0.00,0.05,0.74,0.99,0.99,0.76,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.99,1.00,0.11,0.00,0.36,0.99,1.00,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.99,0.99,0.40,0.46,0.95,0.99,0.42,0.40,0.99,0.85,0.15,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.65,0.99,0.99,0.99,0.96,0.46,0.00,0.07,0.69,0.99,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.49,0.99,0.95,0.75,0.29,0.00,0.00,0.00,0.22,0.99,0.80,0.11,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.44,0.29,0.00,0.00,0.00,0.00,0.00,0.10,0.80,0.99,0.99,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image3[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.29,0.64,0.71,0.82,0.64,0.51,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.13,0.57,0.93,0.99,1.00,0.91,0.96,0.87,0.99,0.33,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.15,0.91,0.92,0.85,0.72,0.40,0.09,0.12,0.12,0.99,0.36,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.39,0.59,0.28,0.00,0.00,0.00,0.00,0.00,0.25,0.99,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.93,0.99,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.77,0.97,0.33,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.64,0.99,0.84,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.77,0.99,0.97,0.23,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.76,1.00,0.99,0.91,0.43,0.30,0.18,0.22,0.06,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.17,0.99,1.00,0.99,0.99,0.99,0.99,1.00,0.99,0.95,0.71,0.46,0.36,0.03,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.26,0.36,0.44,0.66,0.64,0.81,0.71,0.77,1.00,1.00,1.00,1.00,1.00,0.47,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.00,0.02,0.25,0.58,0.78,0.84,0.99,0.98,0.40,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.04,0.93,0.99,0.87,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.20,0.88,0.98,0.22,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.28,0.95,0.94,0.08,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.08,0.69,0.95,0.41,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.28,0.69,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.34,0.82,0.99,0.57,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.95,0.85,0.42,0.15,0.15,0.15,0.15,0.47,0.55,0.86,0.99,1.00,0.58,0.02,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.07,0.97,1.00,0.99,0.99,0.99,0.99,1.00,0.99,0.99,0.99,0.94,0.59,0.02,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.33,0.51,0.71,0.99,0.99,0.99,0.76,0.64,0.64,0.36,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image4[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.86,0.32,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.85,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.07,0.47,0.04,0.00,0.00,0.85,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,1.00,0.28,0.00,0.00,0.85,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,1.00,0.28,0.00,0.00,0.85,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.40,1.00,0.39,0.00,0.00,0.76,0.67,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.55,1.00,0.31,0.00,0.00,0.50,0.92,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.87,0.98,0.24,0.00,0.00,0.50,0.92,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.52,1.00,0.98,0.53,0.43,0.43,0.84,0.64,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.93,1.00,1.00,1.00,1.00,1.00,1.00,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.93,1.00,0.77,0.33,0.23,0.23,0.91,0.57,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.73,0.30,0.02,0.00,0.00,0.13,0.93,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.14,0.95,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,1.00,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,1.00,0.56,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.32,1.00,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.57,1.00,0.21,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.57,1.00,0.21,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.57,0.88,0.05,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.45,0.47,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image5[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.58,0.99,0.99,0.99,0.45,0.44,0.44,0.40,0.25,0.45,0.44,0.56,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.19,0.89,0.98,0.99,0.99,0.99,0.99,0.99,0.99,0.98,0.94,0.99,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.55,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.07,0.84,0.99,0.99,0.90,0.87,0.87,0.94,0.87,0.87,0.82,0.33,0.33,0.33,0.33,0.11,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.74,0.99,0.80,0.10,0.00,0.00,0.25,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.99,1.00,0.66,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.99,0.99,0.65,0.00,0.04,0.33,0.00,0.00,0.07,0.30,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.24,0.99,0.99,0.73,0.22,0.61,0.99,0.78,0.77,0.82,0.96,0.77,0.78,0.59,0.05,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.66,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.61,0.04,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.13,0.84,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.22,0.99,0.99,1.00,0.99,0.99,0.70,0.55,0.55,0.37,0.44,0.55,0.55,1.00,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.65,0.65,0.66,0.16,0.11,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.81,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.44,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.50,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.31,0.99,0.99,0.99,0.33,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.15,0.44,0.15,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.10,0.81,0.99,1.00,0.99,0.89,0.18,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.33,0.99,0.92,0.88,0.33,0.33,0.33,0.33,0.33,0.33,0.51,0.91,0.99,0.99,0.99,0.69,0.08,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.33,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.89,0.20,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.30,0.90,0.93,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.98,0.57,0.18,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.10,0.20,0.44,0.44,0.44,0.56,0.99,0.99,0.99,0.99,0.99,0.57,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image6[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.89,0.71,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.76,0.99,0.31,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.96,0.51,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.91,0.76,0.06,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.69,0.99,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.29,0.97,0.80,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.71,0.95,0.26,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.49,1.00,0.60,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.73,0.97,0.14,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.45,0.99,0.46,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.78,0.89,0.13,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.13,0.98,0.64,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.99,0.64,0.00,0.00,0.00,0.00,0.00,0.00,0.21,0.80,0.85,0.73,0.05,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.99,0.49,0.00,0.00,0.00,0.00,0.00,0.65,0.96,0.92,0.93,0.99,0.76,0.03,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.99,0.53,0.00,0.00,0.00,0.00,0.13,0.89,0.25,0.00,0.02,0.66,1.00,0.45,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.86,0.87,0.11,0.00,0.00,0.00,0.36,0.49,0.00,0.00,0.00,0.00,1.00,0.78,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.50,0.99,0.62,0.09,0.00,0.00,0.03,0.03,0.00,0.00,0.00,0.00,1.00,0.78,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.65,1.00,0.84,0.32,0.03,0.00,0.00,0.00,0.00,0.01,0.40,1.00,0.42,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.44,0.91,0.99,0.82,0.78,0.68,0.43,0.60,0.80,0.99,0.82,0.04,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.06,0.46,0.79,0.99,1.00,0.99,0.99,0.71,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image7[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.53,0.22,0.07,0.07,0.07,0.07,0.07,0.15,0.53,0.18,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.16,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.16,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.29,0.71,0.62,0.25,0.63,0.83,0.99,0.99,0.99,0.99,0.91,0.12,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,0.97,0.99,0.99,0.91,0.24,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.96,0.99,0.99,0.97,0.20,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.73,0.99,0.99,0.76,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.09,0.44,0.56,0.56,0.98,0.99,0.99,0.36,0.09,0.09,0.09,0.09,0.09,0.09,0.09,0.09,0.09,0.05,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.12,0.94,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.93,0.66,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.20,0.43,0.84,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.84,0.76,0.61,0.18,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.99,0.95,0.90,0.76,0.69,0.27,0.23,0.23,0.08,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.99,0.65,0.11,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.99,0.37,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.90,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.95,0.22,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.99,0.37,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.99,0.37,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.92,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,0.99,0.99,0.90,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.48,0.53,0.27,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image8[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.45,0.67,0.78,1.00,1.00,0.55,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.55,1.00,1.00,1.00,1.00,1.00,1.00,0.67,0.00,0.00,0.00,0.34,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,1.00,1.00,0.78,0.11,0.22,0.00,0.67,0.22,0.00,0.00,0.00,0.89,1.00,0.22,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.34,1.00,1.00,0.45,0.00,0.00,0.00,0.00,0.78,0.34,0.00,0.00,0.00,1.00,1.00,0.34,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.22,1.00,1.00,0.45,0.00,0.00,0.00,0.00,1.00,0.00,0.00,0.00,0.11,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.89,1.00,1.00,0.78,0.11,0.00,0.00,0.89,0.00,0.00,0.00,0.89,1.00,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.67,1.00,1.00,1.00,0.55,0.00,0.00,0.00,0.00,0.89,1.00,0.67,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.78,1.00,1.00,1.00,0.45,0.00,0.67,1.00,0.78,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.11,0.89,1.00,1.00,0.78,1.00,0.67,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.45,1.00,1.00,1.00,0.22,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.45,1.00,1.00,0.78,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.45,1.00,1.00,1.00,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.78,1.00,0.55,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.78,0.00,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,1.00,0.67,0.00,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,1.00,0.45,0.00,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.34,1.00,0.45,0.45,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.34,1.00,1.00,1.00,1.00,0.34,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,1.00,1.00,1.00,0.55,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.89,1.00,0.45,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

double image9[] = {
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.13,0.80,0.65,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.31,0.66,0.95,0.97,1.00,1.00,0.12,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.54,0.97,0.83,0.68,0.82,1.00,1.00,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.50,1.00,0.46,0.02,0.00,0.07,1.00,1.00,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.35,1.00,0.59,0.02,0.00,0.00,0.07,1.00,1.00,0.11,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.04,0.84,0.87,0.01,0.00,0.00,0.00,0.09,1.00,1.00,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.21,0.98,0.32,0.00,0.00,0.00,0.00,0.66,1.00,1.00,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.78,0.96,0.00,0.00,0.00,0.00,0.18,0.91,1.00,1.00,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.79,0.96,0.00,0.00,0.00,0.00,0.68,1.00,1.00,1.00,0.04,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.79,0.98,0.53,0.43,0.38,0.59,0.98,0.44,0.75,1.00,0.35,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.29,0.83,1.00,1.00,1.00,1.00,0.44,0.04,0.72,1.00,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.09,0.23,0.35,0.35,0.23,0.04,0.00,0.72,1.00,0.38,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.72,1.00,0.64,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.49,1.00,0.73,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.37,1.00,0.99,0.08,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,1.00,1.00,0.09,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,1.00,1.00,0.27,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.73,1.00,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.67,1.00,0.44,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.31,0.77,0.16,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
    0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
};

class NeuralNodeOperator
{
public:
    inline float activate(float x)
    {
        return xhn::sigmoid(x);
    }
    inline float eval_derivative(float x)
    {
        return xhn::derivative_sigmoid(x);
    }
    inline float learning_rate()
    {
        return 0.1;
    }
    inline float biasing_rate()
    {
        return 0.01;
    }
};

void loadImage(xhn::neural_node_layer<float, 2, NeuralNodeOperator, NeuralNodeOperator>& layer, float* image)
{
    for (euint y = 0; y < 28; y++) {
        for (euint x = 0; x < 28; x++) {
            layer.get_node(x, y)->set_outputted_value(image[y * 28 + x] < 0.01 ? 0.01 : image[y * 28 + x]);
        }
    }
}

void Swap(euint32& value)
{
    euint8* ptr = (euint8*)&value;
    {
        euint8 tmp = ptr[0];
        ptr[0] = ptr[3];
        ptr[3] = tmp;
    }
    {
        euint8 tmp = ptr[1];
        ptr[1] = ptr[2];
        ptr[2] = tmp;
    }
}

class ImageSet
{
public:
    xhn::vector<float*> m_images[10];
    euint m_count = 0;
public:
    ~ImageSet()
    {
        Clear();
    }
    void Clear()
    {
        for (int i = 0; i < 10; i++) {
            for (float* image : m_images[i]) {
                free(image);
            }
            m_images[i].clear();
        }
    }
    void Load(const char* labelsPath,
              const char* imagesPath)
    {
        FILE* labels = fopen(labelsPath, "rb");
        FILE* images = fopen(imagesPath, "rb");
        
        euint32 labelsMagic = 0;
        euint32 numLabels = 0;
        euint32 imagesMagic = 0;
        euint32 numImages = 0;
        euint32 numRows = 0;
        euint32 numColumns = 0;
        fread(&labelsMagic, 1, sizeof(labelsMagic), labels);
        fread(&numLabels, 1, sizeof(numLabels), labels);
        fread(&imagesMagic, 1, sizeof(imagesMagic), images);
        fread(&numImages, 1, sizeof(numImages), images);
        fread(&numRows, 1, sizeof(numRows), images);
        fread(&numColumns, 1, sizeof(numColumns), images);
        Swap(labelsMagic);
        Swap(numLabels);
        Swap(imagesMagic);
        Swap(numImages);
        Swap(numRows);
        Swap(numColumns);
        for (euint32 i = 0; i < numLabels; i++) {
            euint8 label = 0;
            euint8 pixel = 0;
            fread(&label, 1, sizeof(label), labels);
            float* image = (float*)malloc(sizeof(float) * numRows * numColumns);
            for (int y = 0; y < numRows; y++) {
                for (int x = 0; x < numColumns; x++) {
                    fread(&pixel, 1, sizeof(pixel), images);
                    image[y * numRows + x] = (float)pixel / 255.0;
                }
            }
            m_images[label].push_back(image);
        }
        
        fclose(labels);
        fclose(images);
    }
    float* GetImage(int index) {
        return m_images[index][m_count++ % m_images[index].size()];
    }
};

- (void) testNeuralNodeNetwork
{
    float trainValue = 0.9;
    xhn::vector<xhn::layer_config> configs;
    configs.push_back({xhn::InitialConnection, xhn::InitAsZero, 28, 28, 0, 0, 0});
    configs.push_back({xhn::ConvolutionConnection, xhn::InitAsZero, 1, 1, 1, 1, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsZero, 10, 1, 0, 0, 0});
    xhn::neural_node_network<float, 2, NeuralNodeOperator, NeuralNodeOperator> network0;
    network0.setup_layers(configs);
    
    ImageSet imageSet;
    imageSet.Load("/Users/xhnsworks/Downloads/train-labels-idx1-ubyte",
                  "/Users/xhnsworks/Downloads/train-images-idx3-ubyte");
    
    xhn::vector<float> targets[10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i == j) {
                targets[i].push_back(trainValue);
            } else {
                targets[i].push_back(0.01);
            }
        }
    }
    
    int succ[10] = {0};
    int fail[10] = {0};
    
    for (int i = 0; i < 30000; i++) {
        for (int j = 0; j < 10; j++) {
            loadImage(*network0.get_inputted_layer(), imageSet.GetImage(j));
            for (int k = 0; k < 10; k++) {
                network0.get_outputted_layer()->get_node(k, 0)->forward_propagate();
            }
            network0.get_outputted_layer()->descend(targets[j]);
        }
        if (i % 100 == 0) {
            for (int j = 0; j < 10; j++) {
                loadImage(*network0.get_inputted_layer(), imageSet.GetImage(j));
                float value = network0.get_outputted_layer()->get_node(j, 0)->forward_propagate();
                
                float loss = fabsf(trainValue - value);
                bool isFail = false;
                for (int k = 0; k < 10; k++) {
                    if (j != k) {
                        float v = network0.get_outputted_layer()->get_node(k, 0)->forward_propagate();
                        if (loss >= fabsf(trainValue - v)) {
                            fail[j]++;
                            isFail = true;
                            break;
                        }
                    }
                    
                }
                if (!isFail) {
                    succ[j]++;
                }
            }
            printf("///====\n");
            for (int j = 0; j < 10; j++) {
                printf("识别率 %.2f\n", (float)succ[j] / (float)(succ[j] + fail[j]));
            }
        }
    }
    
    int totalSuccCount = 0;
    
    for (int i = 0; i < 10; i++) {
        int succCount = 0;
        for (int j = 0; j < 100; j++) {
            loadImage(*network0.get_inputted_layer(), imageSet.GetImage(i));
            float value = network0.get_outputted_layer()->get_node(i, 0)->forward_propagate();
            
            float loss = fabsf(trainValue - value);
            bool isFail = false;
            for (int k = 0; k < 10; k++) {
                if (i != k) {
                    float v = network0.get_outputted_layer()->get_node(k, 0)->forward_propagate();
                    if (loss >= fabsf(trainValue - v)) {
                        isFail = true;
                        break;
                    }
                }
            }
            if (!isFail) {
                succCount++;
            }
        }
        
        
        printf("识别率 百分之 %d\n", succCount);
        totalSuccCount += succCount;
    }
    
    printf("总识别率 百分之 %d\n", totalSuccCount / 10);
}

class ReLuNeuralNodeOperator
{
public:
    inline float activate(float x)
    {
        if (x >= 0.0f) {
            return x;
        } else {
            return 0.0f;
        }
    }
    inline float eval_derivative(float x)
    {
        if (x > 0.0f)
            return 1.0f;
        else
            return 0.0f;
    }
    inline float learning_rate()
    {
        return 0.05f;
    }
    inline float biasing_rate()
    {
        return 0.05f;
    }
};

class SigmNeuralNodeOperator
{
public:
    inline float activate(float x)
    {
        return xhn::sigmoid(x);
    }
    inline float eval_derivative(float x)
    {
        return xhn::derivative_sigmoid(x);
    }
    inline float learning_rate()
    {
        return 0.5;
    }
    inline float biasing_rate()
    {
        return 0.5;
    }
};

- (void) testNeuralNodeNetwork2
{
    xhn::logger<xhn::CLoggerImpl> logger;
    xhn::vector<xhn::layer_config> configs;
    configs.push_back({xhn::InitialConnection, xhn::InitAsOne, 1, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 8, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 4, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 1, 0, 0, 0, 0});
    xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator> network0;
    network0.setup_layers(configs);
    
//
//    logger.log("Layer0:");
//    {
//        auto* layer = network0.get_layer(0);
//        layer->get_node(0)->log(logger);
//    }
//    logger.log("Layer1:");
//    {
//        auto* layer = network0.get_layer(1);
//        for (euint i = 0; i < 8; i++) {
//            layer->get_node(i)->log(logger);
//        }
//    }
//    logger.log("Layer2:");
//    {
//        auto* layer = network0.get_layer(2);
//        layer->get_node(0)->log(logger);
//    }
    
    xhn::vector<float> targets;
    targets.resize(1);
    for (int i = 0; i < 1000; i++)
    for (float j = 0.0f; j <= 1.0f; j += 0.001f) {
        targets[0] = (1.0f - j) * 0.9f + j * 0.05f;
        network0.get_inputted_layer()->get_node(0)->set_outputted_value(j);
        network0.get_outputted_layer()->get_node(0)->forward_propagate();
        network0.get_outputted_layer()->descend(targets);
        
//        targets[0] = 0.5f;
//        network0.get_inputted_layer()->get_node(0)->set_outputted_value(0.5f);
//        network0.get_outputted_layer()->get_node(0)->forward_propagate();
//        network0.get_outputted_layer()->descend(targets);
//
//        targets[0] = 0.1f;
//        network0.get_inputted_layer()->get_node(0)->set_outputted_value(0.9f);
//        network0.get_outputted_layer()->get_node(0)->forward_propagate();
//        network0.get_outputted_layer()->descend(targets);
        
//        targets[0] = 0.9f;
//        network0.get_inputted_layer()->get_node(0)->set_outputted_value(0.5f);
//        network0.get_outputted_layer()->get_node(0)->forward_propagate();
//        network0.get_outputted_layer()->descend(targets);
//
//        targets[0] = 0.1f;
//        network0.get_inputted_layer()->get_node(0)->set_outputted_value(1.0f);
//        network0.get_outputted_layer()->get_node(0)->forward_propagate();
//        network0.get_outputted_layer()->descend(targets);
    }
    //xhn::logger<xhn::CLoggerImpl> logger;
//    logger.log("Layer0:");
//    {
//        logger.push();
//        auto* layer = network0.get_layer(0);
//        layer->get_node(0)->log(logger);
//        logger.pop();
//    }
//    logger.log("Layer1:");
//    {
//        logger.push();
//        auto* layer = network0.get_layer(1);
//        for (euint i = 0; i < 8; i++) {
//            layer->get_node(i)->log(logger);
//        }
//        logger.pop();
//    }
//    logger.log("Layer2:");
//    {
//        logger.push();
//        auto* layer = network0.get_layer(2);
//        layer->get_node(0)->log(logger);
//        logger.pop();
//    }
    {
        auto* layer = network0.get_layer(0);
        layer->log(logger);
    }
    {
        auto* layer = network0.get_layer(1);
        layer->log(logger);
    }
    {
        auto* layer = network0.get_layer(2);
        layer->log(logger);
    }
    {
        auto* layer = network0.get_layer(3);
        layer->log(logger);
    }
    
    for (float j = 0.0f; j <= 1.0f; j += 0.1f) {
        network0.get_inputted_layer()->get_node(0)->set_outputted_value(j);
        float value = network0.get_outputted_layer()->get_node(0)->forward_propagate();
        printf("func(%f) = %f\n", j, value);
    }
}

void SetupNeuralNetwork(xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator>& network)
{
    xhn::vector<xhn::layer_config> configs;
    configs.push_back({xhn::InitialConnection, xhn::InitAsOne, 1, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 8, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 4, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsRandomNegativeOneToOne, 1, 0, 0, 0, 0});
    network.setup_layers(configs);
}

void TrainNeuralNetwork(xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator>& network,
                        float a, float b)
{
    xhn::vector<float> targets;
    targets.resize(1);
    for (int i = 0; i < 1000; i++) {
        for (float j = 0.0f; j <= 1.0f; j += 0.001f) {
            targets[0] = (1.0f - j) * a + j * b;
            network.get_inputted_layer()->get_node(0)->set_outputted_value(j);
            network.get_outputted_layer()->get_node(0)->forward_propagate();
            network.get_outputted_layer()->descend(targets);
        }
    }
}

- (void) testNeuralNodeNetwork3
{
    xhn::logger<xhn::CLoggerImpl> logger;
    
    xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator> network0;
    xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator> network1;
    
    SetupNeuralNetwork(network0);
    SetupNeuralNetwork(network1);
    
    TrainNeuralNetwork(network0, 0.9f, 0.1f);
    TrainNeuralNetwork(network1, 0.1f, 0.9f);
    
    for (float j = 0.0f; j <= 1.01f; j += 0.1f) {
        network0.get_inputted_layer()->get_node(0)->set_outputted_value(j);
        network1.get_inputted_layer()->get_node(0)->set_outputted_value(j);
        float value0 = network0.get_outputted_layer()->get_node(0)->forward_propagate();
        float value1 = network1.get_outputted_layer()->get_node(0)->forward_propagate();
        printf("func(%f) = %f\n", j, value0 * value1);
    }
}

- (void) testNeuralNodeNetwork4
{
    xhn::vector<xhn::layer_config> configs;
    configs.push_back({xhn::InitialConnection, xhn::InitAsOne, 8, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsOne, 8, 0, 0, 0, 0});
    configs.push_back({xhn::FullConnection, xhn::InitAsOne, 1, 0, 0, 0, 0});
    xhn::neural_node_network<float, 1, ReLuNeuralNodeOperator, SigmNeuralNodeOperator> network0;
    network0.setup_layers(configs);
    
    xhn::vector<float> inputs[8];
    xhn::vector<float> targets[8];
    for (int i = 0; i < 8; i++) {
        inputs[i].resize(8);
        targets[i].resize(1);
        for (int j = 0; j < 8; j++) {
            if (i == j) {
                inputs[i][j] = 0.9f;
            } else {
                inputs[i][j] = 0.05f;
            }
        }
    }
    targets[0][0] = 0.1f;
    targets[1][0] = 0.2f;
    targets[2][0] = 0.3f;
    targets[3][0] = 0.4f;
    targets[4][0] = 0.4f;
    targets[5][0] = 0.3f;
    targets[6][0] = 0.2f;
    targets[7][0] = 0.1f;
    
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                network0.get_inputted_layer()->get_node(k)->set_outputted_value(inputs[j][k]);
            }
            network0.get_outputted_layer()->get_node(0)->forward_propagate();
            network0.get_outputted_layer()->descend(targets[j]);
        }
    }
    
    for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 8; k++) {
            network0.get_inputted_layer()->get_node(k)->set_outputted_value(inputs[j][k]);
        }
        float value = network0.get_outputted_layer()->get_node(0)->forward_propagate();
        printf("%f\n", value);
    }
}

@end
