#include <xhnSTL/common.h>
#include <xhnSTL/etypes.h>
#include <xhnSTL/xhn_lock.hpp>
#include <xhnSTL/xhn_thread.hpp>
#include <xhnSTL/eassert.h>
#include <xhnSTL/xhn_atomic_operation.hpp>
void testAtomicOps() {
    volatile esint32 key = 0;
    EAssert(AtomicIncrement(&key) == 1, "key != 1");
    EAssert(AtomicDecrement(&key) == 0, "key != 0");
    AtomicCompareExchange(1, 0, &key);
    EAssert(AtomicIncrement(&key) == 2, "key != 2");
}
void testVector() {

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
}

class Test : public RefObject
{
public:
    int a;
public:
    ~Test()
    {
        printf("~~~dest test\n");
    }
};
typedef xhn::SmartPtr<Test> TestPtr;
typedef xhn::WeakPtr<Test> TestWeakPtr;

void testWeakPtr() {
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
        EAssert(t.get() != nullptr, "t.get() == nullptr");
    }
    {
        TestPtr t = wtest.ToStrongPtr();
        printf("second t.get() == %p\n", t.get());
        EAssert(t.get() == nullptr, "t.get() != nullptr");
    }
}

int main(int argc, char *argv[])
{
    xhn::thread_ptr t = VNEW xhn::thread();
    while(!t->is_running()) {}
    volatile int completed = 0;

    xhn::Lambda<xhn::thread::TaskStatus()> testAtomicOpsProc([&completed](){
        testAtomicOps();
        printf("passed test atomic ops!\n");
        completed += 1;
        return xhn::thread::Completed;
    });

    xhn::Lambda<xhn::thread::TaskStatus()> testVectorProc([&completed](){
        testVector();
        printf("passed test vector!\n");
        completed += 1;
        return xhn::thread::Completed;
    });

    xhn::Lambda<xhn::thread::TaskStatus()> testWeakPtrProc([&completed](){
        testWeakPtr();
        printf("passed test weak ptr!\n");
        completed += 1;
        return xhn::thread::Completed;
    });

    t->add_lambda_task(testAtomicOpsProc);
    t->add_lambda_task(testVectorProc);
    t->add_lambda_task(testWeakPtrProc);

    while (completed < 3) {}
    return 0;
}
