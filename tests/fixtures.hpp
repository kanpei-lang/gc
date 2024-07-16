#ifndef __KANPEI_MEMORY_GC_TESTS_FIXTURES
#define __KANPEI_MEMORY_GC_TESTS_FIXTURES

#include <gtest/gtest.h>

#include "../src/kanpei/memory/gc/all.hpp"
#include "memory.hpp"

class KanpeiGcTests : public ::testing::Test {
   public:
    static void SetUpTestSuite() {
        heap_pointers = new std::unordered_set<
            void *, std::hash<void *>, std::equal_to<void *>, StdMallocAllocator<void *>>();
        garb_coll = new collector();

        garb_coll->collect_forever();
    }

    static void TearDownTestSuite() {
        /* allow excess time for the garbage collector to finish sweeping.
            otherwise, the coverage analysis will be very non-deterministic and
            dependent on how quickly the system starts the sweep thread */
        std::this_thread::sleep_for(std::chrono::seconds(2));

        delete heap_pointers;
        delete garb_coll;

        heap_pointers = nullptr;
        garb_coll = nullptr;
    }
};

#endif /* __KANPEI_MEMORY_GC_TESTS_FIXTURES */