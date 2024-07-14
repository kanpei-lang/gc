#include "garbage_collector.hpp"

using namespace kanpei::memory;

garbage_collector::~garbage_collector() {
    if (this->collect_thread.joinable()) {
        this->stop_collect_thread = true;
        this->collect_thread.join();
    }
}

void garbage_collector::add_reference(void *object) {
    std::scoped_lock lock(this->object_map_mutex);

    if (!this->objects.contains(object)) {
        this->objects.insert_or_assign(object, 0);
    }

    this->objects.at(object) += 1;
}

void garbage_collector::collect() {
    unsigned long freed_count = 0;
    {
        /* acquire a lock on the refcount hash map */
        std::scoped_lock lock(this->object_map_mutex);

        /* loop over all of the objects and free
            anything with a refcount of 0 */
        for (auto &pair : this->objects) {
            if (pair.second <= 0) {
                free(pair.first);
                this->objects.erase(pair.first);

                freed_count += 1;
            }
        }
    }

    /* if we didn't free anything, sleep for 10 millis */
    if (freed_count == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void garbage_collector::collect_forever() {
    this->collect_thread = std::thread([&](garbage_collector *gc) { gc->collect_loop(); }, this);
}

void garbage_collector::collect_loop() {
    while (!this->stop_collect_thread) {
        this->collect();
    }
}

void garbage_collector::remove_reference(void *object) {
    std::scoped_lock lock(this->object_map_mutex);

    this->objects.at(object) -= 1;
}
