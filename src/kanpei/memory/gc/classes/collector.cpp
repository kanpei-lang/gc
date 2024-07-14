#include "collector.hpp"

using namespace kanpei::memory::gc;

collector::~collector() {
    if (this->collect_thread.joinable()) {
        this->stop_collect_thread = true;
        this->collect_thread.join();
    }
}

void collector::collect() {
    unsigned long freed_count = 0;

    freed_count += this->mark();

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

unsigned long collector::mark() {
    unsigned long finalized_count = 0;
    {
        /* acquire a lock on the refcount hash map */
        std::scoped_lock lock(this->object_map_mutex);

        /* loop over all of the objects and free
            anything with a refcount of 0 */
        for (auto &pair : this->objects) {
            if (pair.second.refcount <= 0) {
                if (!pair.second.is_primitive) {
                    ((managed_object *)pair.first)->~managed_object();
                }
                free(pair.first);
                this->objects.erase(pair.first);

                finalized_count += 1;
            }
        }
    }

    return finalized_count;
}
