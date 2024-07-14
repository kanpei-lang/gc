#include "collector.hpp"

#include <iostream>

#include "managed_object.hpp"

using namespace kanpei::memory::gc;

collector::~collector() {
    if (this->collect_thread.joinable()) {
        this->stop_collect_thread = true;
        this->collect_thread.join();
    }
}

void collector::add_reference(i_managed &object) {
    std::scoped_lock lock(this->object_map_mutex);

    this->objects.insert(&object);
    object.refcount++;
}

void collector::collect() {
    unsigned long freed_count = 0;

    // freed_count += this->mark(this->objects);

    /* if we didn't free anything, sleep for 10 millis */
    if (freed_count == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void collector::collect_forever() {
    this->collect_thread = std::thread([&](collector *gc) { gc->collect_loop(); }, this);
}

void collector::collect_loop() {
    while (!this->stop_collect_thread) {
        this->collect();
    }
}

void collector::finalize(i_managed &object) {
    std::cout << "finalize " << &object << std::endl;

    delete &object;
    /*if (!object.is_primitive) {
        delete &object;
    } else {
        free(&object);
    }*/

    objects.erase(&object);
}

/*unsigned long collector::mark(i_managed_set &objects) {
    unsigned long marked_count = 0;
    {
        /* acquire a lock on the refcount hash map */
/*std::scoped_lock lock(object_map_mutex);

/* loop over all of the objects and mark anything with
    with a refcount of 0 */
/*for (auto managed_ptr : objects) {
    if (managed_ptr->refcount == 0) {
        if (!managed_ptr->is_primitive) {
            this->mark(managed_ptr->references);
            delete managed_ptr;
        } else {
            free(managed_ptr);
        }

        objects.erase(managed_ptr);

        marked_count += 1;
    }
}
}

return marked_count;
}*/

void collector::remove_reference(i_managed &object) {
    std::scoped_lock lock(this->object_map_mutex);

    if (--object.refcount == 1) {
        this->finalize(object);
    }
}

void collector::sweep() {
    std::cout << "Sweep!" << std::endl;
}

void collector::sweep_recurse() {}
