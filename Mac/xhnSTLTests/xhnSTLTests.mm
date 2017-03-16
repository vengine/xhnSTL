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
- (void) testVector {
    xhn::vector<int> intArray;
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
/**
- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}
 **/

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
