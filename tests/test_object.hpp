#ifndef TEST_OBJECT
#define TEST_OBJECT

#include "../src/kanpei/memory/gc/all.hpp"

extern kanpei::memory::gc::collector *garb_coll;

class test_object : public kanpei::memory::gc::managed_object {
   public:
    test_object() : kanpei::memory::gc::managed_object(garb_coll) {}

    int some_method() {
        return -123456;
    }
};

#endif  // TEST_OBJECT