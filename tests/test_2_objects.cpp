#include <vector>

#include "fixtures.hpp"
#include "test_object.hpp"

using namespace kanpei::memory::gc;

/* the amount of time tests should wait for a sweep to occur.
    the sweep is usually much quicker than this, but transient
    conditions can occur that make the tests unreliable if this
    time is much shorter */
#define SWEEP_PAUSE_MILLISECONDS 2000

/* check if allocating/freeing managed objects is working */
TEST_F(KanpeiGcTests, AllocateObject) {
    test_object *outer_scoped_ptr;
    {
        ref<test_object> test = ref<test_object>(new test_object);
        outer_scoped_ptr = &*test;

        /* ensure that the managed_object is marked allocated */
        ASSERT_TRUE(is_allocated(&*test)) << "Object address not found in heap set";
    }

    ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Object address still found in heap set";
}

/* check if allocating/freeing many managed objects is working */
TEST_F(KanpeiGcTests, AllocateManyObjects) {
    for (int n = 0; n < 100000; n++) {
        test_object *outer_scoped_ptr;
        {
            ref<test_object> test = ref<test_object>(new test_object);
            outer_scoped_ptr = &*test;

            /* ensure that the managed_object is marked allocated */
            ASSERT_TRUE(is_allocated(&*test)) << "Object address not found in heap set";
        }

        ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Object address still found in heap set";
    }
}

/* check if allocating/freeing managed objects that contain primitives is working */
TEST_F(KanpeiGcTests, AllocateObjectWithPrimitive) {
    test_object *outer_scoped_ptr;
    int *outer_scoped_primitive_ptr;
    {
        ref<test_object> test = ref<test_object>(new test_object);
        outer_scoped_ptr = &*test;

        /* allocate the primitive and reference it from the object */
        ref<managed_ptr<int>> test_primitive = garb_coll->allocate<int>();
        outer_scoped_primitive_ptr = &**test_primitive;

        /* ensure that the objects are marked allocated */
        ASSERT_TRUE(is_allocated(outer_scoped_ptr)) << "Object address not found in heap set";
        ASSERT_TRUE(is_allocated(outer_scoped_primitive_ptr))
            << "Primitive address not found in heap set";
    }

    ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Object address still found in heap set";
    ASSERT_TRUE(is_freed(outer_scoped_primitive_ptr))
        << "Primitive address still found in heap set";
}

/* check if allocating/freeing managed objects that contain other objects is working */
TEST_F(KanpeiGcTests, AllocateObjectWithChild) {
    test_object *outer_scoped_ptr;
    {
        ref<test_object> outer = ref<test_object>(new test_object);
        outer_scoped_ptr = &*outer;

        test_object *inner_scoped_ptr;
        {
            ref<test_object> inner = ref<test_object>(new test_object);
            inner_scoped_ptr = &*inner;

            ASSERT_TRUE(is_allocated(inner_scoped_ptr))
                << "Inner object address not found in heap set";
            ASSERT_TRUE(is_allocated(outer_scoped_ptr))
                << "Outer object address not found in heap set";

            /* try adding/remove the inner object a few different ways */
            outer->add_reference(*inner);
            outer->remove_reference(*inner);
            outer->add_reference(&*inner);
            outer->remove_reference(&*inner);
            outer->add_reference(inner);
            outer->remove_reference(inner);

            ASSERT_TRUE(is_allocated(inner_scoped_ptr))
                << "Inner object address not found in heap set after removing reference";
        }

        ASSERT_TRUE(is_freed(inner_scoped_ptr)) << "Inner object address still found in heap set";
    }

    ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Inner object address still found in heap set";
}

/* check if cycle detection for self-referential objects is working */
TEST_F(KanpeiGcTests, AllocateObjectSelfReferential) {
    test_object *outer_scoped_ptr;
    {
        ref<test_object> test = ref<test_object>(new test_object);
        outer_scoped_ptr = &*test;

        /* make the object self-referential. add the reference multiple times
            to check if add_reference() is idempotent */
        test->add_reference(*test);

        /* ensure that the managed_object is marked allocated */
        ASSERT_TRUE(is_allocated(&*test)) << "Object address not found in heap set";
    }

    /* give the garbage collector some time to sweep then check if the object was freed.
        note that this doesn't guarantee the test will pass if the garbage collector is
        configured with too low affinity or if its thread is too slow */
    std::this_thread::sleep_for(std::chrono::milliseconds(SWEEP_PAUSE_MILLISECONDS));
    ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Object address still found in heap set";

    /* check a second time, but add the reference multiple times to check if add_reference()
        is idempotent or not */
    {
        ref<test_object> test = ref<test_object>(new test_object);
        outer_scoped_ptr = &*test;

        /* make the object self-referential. add the reference multiple times
            to check if add_reference() is idempotent */
        for (int n = 0; n < 10; n++) {
            test->add_reference(*test);
        }

        /* ensure that the managed_object is marked allocated */
        ASSERT_TRUE(is_allocated(&*test)) << "Object address not found in heap set";
    }

    /* give the garbage collector some time to sweep then check if the object was freed.
        see above note on sleeping this thread */
    std::this_thread::sleep_for(std::chrono::milliseconds(SWEEP_PAUSE_MILLISECONDS));
    ASSERT_TRUE(is_freed(outer_scoped_ptr)) << "Object address still found in heap set";
}

/* check if cycle detection is working for two object cycles */
TEST_F(KanpeiGcTests, AllocateObjectsSimpleCycle) {
    test_object *outer_scoped_ptr_a;
    test_object *outer_scoped_ptr_b;
    {
        ref<test_object> object_a = ref<test_object>(new test_object);
        ref<test_object> object_b = ref<test_object>(new test_object);
        outer_scoped_ptr_a = &*object_a;
        outer_scoped_ptr_b = &*object_b;

        /* make the two objects reference each other to create a cycle */
        object_a->add_reference(*object_a);
        object_a->add_reference(*object_b);
        object_b->add_reference(*object_a);

        ASSERT_TRUE(is_allocated(outer_scoped_ptr_a))
            << "Object address for object_a not found in heap set";
        ASSERT_TRUE(is_allocated(outer_scoped_ptr_b))
            << "Object address for object_b not found in heap set";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(SWEEP_PAUSE_MILLISECONDS));
    ASSERT_TRUE(is_freed(outer_scoped_ptr_a))
        << "Object address for object_a still found in heap set";
    ASSERT_TRUE(is_freed(outer_scoped_ptr_b))
        << "Object address for object_b still found in heap set";
}

/* check if cycle detection is working for three object cycles */
TEST_F(KanpeiGcTests, AllocateObjectsThreeObjCycle) {
    test_object *outer_scoped_ptr_a;
    test_object *outer_scoped_ptr_b;
    test_object *outer_scoped_ptr_c;
    {
        ref<test_object> object_a = ref<test_object>(new test_object);
        ref<test_object> object_b = ref<test_object>(new test_object);
        ref<test_object> object_c = ref<test_object>(new test_object);
        outer_scoped_ptr_a = &*object_a;
        outer_scoped_ptr_b = &*object_b;
        outer_scoped_ptr_c = &*object_c;

        /* make the two objects reference each other to create a cycle */
        object_a->add_reference(*object_b);
        object_b->add_reference(*object_b);
        object_b->add_reference(*object_c);
        object_c->add_reference(*object_a);

        ASSERT_TRUE(is_allocated(outer_scoped_ptr_a))
            << "Object address for object_a not found in heap set";
        ASSERT_TRUE(is_allocated(outer_scoped_ptr_b))
            << "Object address for object_b not found in heap set";
        ASSERT_TRUE(is_allocated(outer_scoped_ptr_c))
            << "Object address for object_c not found in heap set";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(SWEEP_PAUSE_MILLISECONDS));
    ASSERT_TRUE(is_freed(outer_scoped_ptr_a))
        << "Object address for object_a still found in heap set";
    ASSERT_TRUE(is_freed(outer_scoped_ptr_b))
        << "Object address for object_b still found in heap set";
    ASSERT_TRUE(is_freed(outer_scoped_ptr_c))
        << "Object address for object_c still found in heap set";
}

#define CYCLE_DEPTH 20
TEST_F(KanpeiGcTests, AllocateObjectsDeepCycle) {
    test_object *outer_scoped_ptrs[CYCLE_DEPTH];
    {
        /* allocate the objects that we will create a cycle with */
        std::vector<ref<test_object>> objects;
        for (int n = 0; n < CYCLE_DEPTH; n++) {
            objects.push_back(ref<test_object>(new test_object));
            outer_scoped_ptrs[n] = &*objects.back();

            ASSERT_TRUE(is_allocated(outer_scoped_ptrs[n]))
                << "Object address for object not found in heap set";
        }

        /* make the objects cycle in multiple ways */
        for (int n = 0; n < CYCLE_DEPTH - 1; n++) {
            outer_scoped_ptrs[n]->add_reference(outer_scoped_ptrs[n + 1]);
            for (int factor = 2; factor < 5; factor++) {
                outer_scoped_ptrs[n]->add_reference(outer_scoped_ptrs[(n * factor) % CYCLE_DEPTH]);
            }
        }
        outer_scoped_ptrs[CYCLE_DEPTH - 1]->add_reference(outer_scoped_ptrs[0]);

        /* check that the objects haven't already been freed */
        for (int n = 0; n < CYCLE_DEPTH; n++) {
            ASSERT_TRUE(is_allocated(outer_scoped_ptrs[n]))
                << "Object address for object not found in heap set";
        }
    }

    /* wait a bit for the sweep process to run then check if all pointers are freed */
    std::this_thread::sleep_for(std::chrono::milliseconds(SWEEP_PAUSE_MILLISECONDS * 2));
    for (int n = 0; n < CYCLE_DEPTH; n++) {
        ASSERT_TRUE(is_freed(outer_scoped_ptrs[n]))
            << "Object address for object still found in heap set";
    }
}
