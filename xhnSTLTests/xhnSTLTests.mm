//
//  xhnSTLTests.m
//  xhnSTLTests
//
//  Created by 徐海宁 on 15/1/3.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#include "xhn_vector.hpp"
#include "xhn_set.hpp"
#include "xhn_crypto.hpp"
#include "timer.h"

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

- (void)testExample {
    // This is an example of a functional test case.
    XCTAssert(YES, @"Pass");
}

- (void)testVector {
    
    int ints[] = {0, 1, 2, 3, 4, 5, 6};

    {
        xhn::vector<int> intVec(&ints[0], &ints[6]);
        xhn::vector<int>::iterator iter = intVec.begin();
        xhn::vector<int>::iterator end = intVec.end();
        for (; iter != end; iter++) {
            printf("%d\n", *iter);
        }
    }
    {
        xhn::vector<int> intVec(&ints[0], &ints[6]);
        printf("--------------\n");
        xhn::vector<int>::iterator from = intVec.begin() + 2;
        int tmp = *from;
        intVec.erase(from);
        xhn::vector<int>::iterator to = intVec.end();
        xhn::vector<int>::iterator i = intVec.insert(to, tmp);
        printf("%d\n", *i);
        printf("--------------\n");
        {
            xhn::vector<int>::iterator iter = intVec.begin();
            xhn::vector<int>::iterator end = intVec.end();
            for (; iter != end; iter++) {
                printf("%d\n", *iter);
            }
        }
        printf("--------------\n");
    }
    {
        xhn::vector<int> intVec(&ints[0], &ints[6]);
        printf("--------------\n");
        {
            xhn::vector<int>::iterator iter = intVec.begin();
            xhn::vector<int>::iterator end = intVec.end();
            for (; iter != end; iter++) {
                printf("%d\n", *iter);
            }
        }
        printf("--------------\n");
        xhn::vector<int>::iterator from = intVec.begin() + 2;
        int tmp = *from;
        intVec.erase(from);
        {
            xhn::vector<int>::iterator iter = intVec.begin();
            xhn::vector<int>::iterator end = intVec.end();
            for (; iter != end; iter++) {
                printf("%d\n", *iter);
            }
        }
        printf("--------------\n");
        xhn::vector<int>::iterator to = intVec.begin() + 3;
        xhn::vector<int>::iterator i = intVec.insert(to, tmp);
        printf("%d\n", *i);
        printf("--------------\n");
        {
            xhn::vector<int>::iterator iter = intVec.begin();
            xhn::vector<int>::iterator end = intVec.end();
            for (; iter != end; iter++) {
                printf("%d\n", *iter);
            }
        }
        printf("--------------\n");
    }
    XCTAssert(YES, @"Pass");
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

void MemoryAccessTest(const int MinIndexInterval, const int MaxIndexInterval, xhn::Lambda<void ()>& clearCacheProc)
{
    xhn::sha256 sha0;
    xhn::sha256 sha1;
    int* testedMemory = nullptr;
    int* intIndexs = nullptr;
    int* result = nullptr;
    
    const int TestedIntArraySize = 1024 * 1024 * 128;
    const int TotalNumberOfSamples = 4096;
    
    testedMemory = (int*)SMalloc(sizeof(int) * TestedIntArraySize);
    intIndexs = (int*)SMalloc(sizeof(int) * TotalNumberOfSamples);
    result = (int*)SMalloc(sizeof(int) * TotalNumberOfSamples);
    
    for (int i = 0; i < TestedIntArraySize; i++) {
        testedMemory[i] = rand();
    }
    int mask = 0;
    if (MinIndexInterval < 4096) {
        mask = 0xfff;
    }
    else if (MinIndexInterval >= 4096 && MinIndexInterval < 65536) {
        mask = 0xffff;
    }
    else if (MinIndexInterval >= 65536 && MinIndexInterval < 1048576) {
        mask = 0xfffff;
    }
    else if (MinIndexInterval >= 1048576 && MinIndexInterval < 16777216) {
        mask = 0xffffff;
    }
    else {
        mask = 0xffffffff;
    }
    int lastIndex = 0;
    for (int i = 0; i < TotalNumberOfSamples; i++) {
        int index = lastIndex;
        while (abs(index - lastIndex) <= MinIndexInterval ||
               abs(index - lastIndex) >= MaxIndexInterval ||
               index > TestedIntArraySize- 1) {
            index = rand() & mask;
        }
        sha0.update(testedMemory[index]);
        intIndexs[i] = index;
        lastIndex = index;
    }
    sha0.complete();
    
    VTime t;
    TimeCheckpoint t0 = TimeCheckpoint::Tick();
    clearCacheProc();
    for (int i = 0; i < TotalNumberOfSamples; i++) {
        result[i] = testedMemory[intIndexs[i]];
    }
    
    TimeCheckpoint::Tock(t0, t);
    
    printf("nano %f\n", t.GetNanosecond());
    
    for (int i = 0; i < TotalNumberOfSamples; i++) {
        sha1.update(testedMemory[intIndexs[i]]);
    }
    sha1.complete();
    
    printf("src %s\n", sha0.sha_string().c_str());
    printf("dst %s\n", sha1.sha_string().c_str());
    
    Mfree(testedMemory);
    Mfree(intIndexs);
    Mfree(result);
}

- (void)testMemoryPerformance {
    xhn::Lambda<void ()> clearCacheProc([]() {
        int* clearBuffer = (int*)SMalloc(1024 * 1024 * 16);
        Mfree(clearBuffer);
    });
    printf("L1 Cache Test\n");
    MemoryAccessTest(256, 1024, clearCacheProc);
    printf("L2 Cache Test\n");
    MemoryAccessTest(1024 * 8, 1024 * 32, clearCacheProc);
    printf("L3 Cache Test\n");
    MemoryAccessTest(1024 * 128, 1024 * 512, clearCacheProc);
    printf("Memory Test\n");
    MemoryAccessTest(1024 * 1024 * 4, 1024 * 1024 * 64, clearCacheProc);
}


@end
