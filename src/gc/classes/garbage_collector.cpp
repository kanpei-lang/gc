#include "garbage_collector.hpp"

#include <cstdlib>

using namespace kanpei::memory;

void garbage_collector::add_reference(void *object) {
    if (!this->objects.contains(object)) {
        this->objects.insert_or_assign(object, 0);
    }

    this->objects.at(object) += 1;
}

void garbage_collector::collect() {
    for (auto &pair : this->objects) {
        if (pair.second <= 0) {
            free(pair.first);
            this->objects.erase(pair.first);
        }
    }
}

void garbage_collector::remove_reference(void *object) {
    this->objects.at(object) -= 1;
}
