#ifndef __KANPEI_MEMORY_GC_TESTS_MEMORY
#define __KANPEI_MEMORY_GC_TESTS_MEMORY

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <unordered_set>

#include "../src/kanpei/memory/gc/all.hpp"

template <typename T>
class StdMallocAllocator {
   public:
    using value_type = T;

    StdMallocAllocator() noexcept {}

    template <typename U>
    StdMallocAllocator(const StdMallocAllocator<U> &) noexcept {}

    T *allocate(std::size_t n) {
        if (n == 0) {
            return nullptr;
        }
        if (n > static_cast<std::size_t>(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }

        void *ptr = std::malloc(n * sizeof(T));
        if (!ptr) {
            throw std::bad_alloc();
        }

        return (T *)ptr;
    }

    void deallocate(T *p, std::size_t) noexcept {
        std::free(p);
    }

    template <typename U>
    bool operator==(const StdMallocAllocator<U> &) const noexcept {
        return true;
    }

    template <typename U>
    bool operator!=(const StdMallocAllocator<U> &) const noexcept {
        return false;
    }
};

extern std::unordered_set<
    void *, std::hash<void *>, std::equal_to<void *>, StdMallocAllocator<void *>> *heap_pointers;

extern kanpei::memory::gc::collector *garb_coll;

bool is_allocated(void *ptr);
bool is_freed(void *ptr);
void *tracked_free(std::size_t size);
void *tracked_malloc(std::size_t size);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
void operator delete(void *ptr, std::size_t size) noexcept;
void operator delete[](void *ptr, std::size_t size) noexcept;
void *operator new(std::size_t size);
void *operator new[](std::size_t size);

#endif /* __KANPEI_MEMORY_GC_TESTS_MEMORY */