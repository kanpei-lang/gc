#include "managed_object.hpp"

using namespace kanpei::memory::gc;

managed_object::managed_object(collector *parent) : i_managed(false) {
    this->parent = parent;
    this->is_primitive = false;

    parent->add_reference(*this);
}

managed_object::~managed_object() {
    for (auto reference : this->references) {
        i_managed object = reference;
        this->remove_reference(object);
    }
}

void managed_object::add_reference(i_managed &object) {
    if (this->references.contains(&object)) {
        return;
    }

    this->references.insert(&object);
    this->parent->add_reference(object);
}

void managed_object::add_reference(i_managed *object) {
    if (this->references.contains(object)) {
        return;
    }

    this->references.insert(object);
    this->parent->add_reference(*object);
}

void managed_object::remove_reference(i_managed &object) {
    if (!this->references.contains(&object)) {
        return;
    }

    this->references.erase(&object);
    this->parent->remove_reference(object);
}

void managed_object::remove_reference(i_managed *object) {
    if (!this->references.contains(object)) {
        return;
    }

    this->references.erase(object);
    this->parent->remove_reference(*object);
}
