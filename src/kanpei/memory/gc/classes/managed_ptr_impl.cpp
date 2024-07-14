#ifndef __KANPEI_MEMORY_GC_MANAGED_PTR_IMPL
#define __KANPEI_MEMORY_GC_MANAGED_PTR_IMPL

/* clang-format off */
#include "managed_ptr.hpp"
#include "collector.hpp"
/* clang-format on */

using namespace kanpei::memory::gc;

template <typename T>
managed_ptr<T>::managed_ptr() {
    this->parent = nullptr;
    this->object = nullptr;
}

template <typename T>
managed_ptr<T>::managed_ptr(T *object, collector *parent) {
    this->parent = parent;
    this->object = object;
}

template <typename T>
managed_ptr<T>::managed_ptr(const managed_ptr<T> &other) {
    this->parent = other.parent;
    this->object = other.object;
}

template <typename T>
managed_ptr<T>::~managed_ptr() {
    free(this->object);
}

template <typename T>
T &managed_ptr<T>::operator*() {
    return *this->object;
}

template <typename T>
managed_ptr<T> &managed_ptr<T>::operator=(const managed_ptr<T> &other) {
    this->object = other.object;
    this->parent = other.parent;

    return *this;
}

#endif /* __KANPEI_MEMORY_GC_MANAGED_PTR */
