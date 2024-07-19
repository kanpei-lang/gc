#include "fixtures.hpp"

using namespace kanpei::memory::gc;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"

/* ensures that custom heap allocation with tracking is working */
TEST_F(KanpeiGcTests, CustomAllocation) {
    /* check that the global heap tracking set captures a new pointer */
    int *test = new int;
    ASSERT_TRUE(is_allocated(test)) << "Pointer address not found in heap set";

    /* check that the global heap tracking set removed this pointer */
    delete test;
    ASSERT_TRUE(is_freed(test)) << "Pointer address still found in heap set";
}

#pragma GCC diagnostic pop

/* check if allocating and freeing primitives based on refcount is working */
TEST_F(KanpeiGcTests, AllocatePrimitive) {
    managed_ptr<int> *test_ptr = nullptr;
    int *test_int_ptr = nullptr;
    {
        /* allocate a garbage collected primitive int */
        ref<managed_ptr<int>> test = garb_coll->allocate<int>();
        test_ptr = &*test;

        /* ensure that the int is marked allocated */
        ASSERT_TRUE(is_allocated(&**test_ptr)) << "Pointer address not found in heap set";
        test_int_ptr = &**test_ptr;
    }

    /* ensure the int was freed */
    ASSERT_TRUE(is_freed(test_ptr))
        << "Pointer address for managed_ptr<int> still found in heap set";
    ASSERT_TRUE(is_freed(test_int_ptr)) << "Pointer address for int* still found in heap set";
}

/* check if allocating/freeing primitive arrays based on refcount is working */
TEST_F(KanpeiGcTests, AllocatePrimitiveArray) {
    managed_ptr<int[100]> *test_ptr = nullptr;
    int(*test_int_ptr)[100];
    {
        /* allocate a garbage collected primitive int array */
        ref<managed_ptr<int[100]>> test = garb_coll->allocate<int[100]>();
        test_ptr = &*test;

        /* ensure that the int array is marked allocated */
        ASSERT_TRUE(is_allocated(&**test_ptr)) << "Pointer address not found in heap set";
        test_int_ptr = &**test_ptr;
    }

    /* ensure the int was freed */
    ASSERT_TRUE(is_freed(test_ptr))
        << "Pointer address for managed_ptr<int[]> still found in heap set";
    ASSERT_TRUE(is_freed(test_int_ptr)) << "Pointer address for int[] still found in heap set";
}

TEST_F(KanpeiGcTests, AllocateManyPrimitives) {
    for (int n = 0; n < 100000; n++) {
        managed_ptr<int> *test_ptr = nullptr;
        int *test_int_ptr = nullptr;
        {
            /* allocate a garbage collected primitive int */
            ref<managed_ptr<int>> test = garb_coll->allocate<int>();
            test_ptr = &*test;

            /* ensure that the int is marked allocated */
            ASSERT_TRUE(is_allocated(&**test_ptr)) << "Pointer address not found in heap set";
            test_int_ptr = &**test_ptr;
        }

        /* ensure the int was freed */
        ASSERT_TRUE(is_freed(test_ptr))
            << "Pointer address for managed_ptr<int> still found in heap set";
        ASSERT_TRUE(is_freed(test_int_ptr)) << "Pointer address for int* still found in heap set";
    }
}