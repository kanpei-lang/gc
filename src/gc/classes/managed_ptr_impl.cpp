#ifndef __GC_CLASSES_MANAGED_PTR_IMPL
#define __GC_CLASSES_MANAGED_PTR_IMPL

// clang-format off
#include "managed_ptr.hpp"
#include "garbage_collector.hpp"
// clang-format on

using namespace kanpei::memory;

template <typename T>
managed_ptr<T>::managed_ptr(T *object, garbage_collector *parent) {
    this->parent = parent;
    this->object = object;

    parent->add_reference(object);
}

template <typename T>
managed_ptr<T>::managed_ptr(const managed_ptr<T> &other) {
    this->parent = other.parent;
    this->object = other.object;

    this->parent->add_reference(this->object);
}

template <typename T>
managed_ptr<T>::~managed_ptr() {
    this->parent->remove_reference(this->object);
}

template <typename T>
T &managed_ptr<T>::operator*() {
    return *this->object;
}

template <typename T>
managed_ptr<T> &managed_ptr<T>::operator=(const managed_ptr<T> &other) {
    this->object = other.object;
    this->parent = other.parent;
}

#endif  // __GC_CLASSES_MANAGED_PTR_IMPL
