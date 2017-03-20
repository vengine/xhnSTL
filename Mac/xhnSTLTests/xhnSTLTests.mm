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
#include <map>
#include <unordered_map>

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
    __block xhn::dictionary<xhn::static_string, int> hash_table;
    [self measureBlock:^{
        for (int i = 0; i < num; i++) {
            hash_table.insert(strs[i], i);
        }
    }];
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
    __block xhn::dictionary<xhn::static_string, int> hash_table;
    for (int i = 0; i < num; i++) {
        hash_table.insert(strs[i], i);
    }
    [self measureBlock:^{
        int value = 0;
        for (int i = 0; i < num; i++) {
            int* v = hash_table.find(strs[i]);
            value += *v;
        }
    }];
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



@end
