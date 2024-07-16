#include "memory.hpp"

using namespace kanpei::memory::gc;

/* global set to contain allocated heap pointers */
std::unordered_set<void *, std::hash<void *>, std::equal_to<void *>, StdMallocAllocator<void *>>
    *heap_pointers = nullptr;

/* global garbage collector for tests */
collector *garb_coll = nullptr;

/* global mutex for accessing the pointer set */
std::recursive_mutex heap_pointers_mutex;

bool is_allocated(void *ptr) {
    std::lock_guard lock(heap_pointers_mutex);

    if (heap_pointers != nullptr && heap_pointers->find(ptr) != heap_pointers->end()) {
        return true;
    }

    return false;
}

bool is_freed(void *ptr) {
    std::lock_guard lock(heap_pointers_mutex);

    if (heap_pointers != nullptr && heap_pointers->find(ptr) == heap_pointers->end()) {
        return true;
    }

    return false;
}

void tracked_free(void *ptr) {
    std::lock_guard lock(heap_pointers_mutex);

    if (heap_pointers != nullptr) {
        heap_pointers->erase(ptr);
    }

    return free(ptr);
}

void *tracked_malloc(std::size_t size) {
    std::lock_guard lock(heap_pointers_mutex);

    void *ptr = malloc(size);

    if (heap_pointers != nullptr) {
        heap_pointers->insert(ptr);
    }

    return ptr;
}

void operator delete(void *ptr) noexcept {
    tracked_free(ptr);
}

void operator delete[](void *ptr) noexcept {
    tracked_free(ptr);
}

void operator delete(void *ptr, std::size_t) noexcept {
    tracked_free(ptr);
}

void operator delete[](void *ptr, std::size_t) noexcept {
    tracked_free(ptr);
}

void *operator new(std::size_t size) {
    if (void *ptr = tracked_malloc(size)) {
        return ptr;
    }

    /* throw bad_alloc if we got nullptr back from malloc() */
    throw std::bad_alloc();
}

void *operator new[](std::size_t size) {
    if (void *ptr = tracked_malloc(size)) {
        return ptr;
    }

    /* throw bad_alloc if we got nullptr back from malloc() */
    throw std::bad_alloc();
}