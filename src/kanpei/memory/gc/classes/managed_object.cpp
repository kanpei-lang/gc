#include "managed_object.hpp"

using namespace kanpei::memory::gc;

managed_object::managed_object(collector *parent) : i_managed(false) {
    this->parent = parent;
    this->is_primitive = false;

    parent->add_reference(*this);
}

void managed_object::add_reference(i_managed &object) {
    this->references.insert(&object);
    object.refcount++;
}

void managed_object::remove_reference(i_managed &object) {
    object.refcount--;
}
