#include "collector.hpp"

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

unsigned long collector::collect() {
    return this->sweep();
}

void collector::collect_forever() {
    /* NOTE: this check is here to make calls to collect_forever() idempotent.
        Specifically in tests, each test suite may try to restart a collector
        instance. Without this check, the running thread would be freed, resulting
        in hard to trace 'abort without exception' errors */
    if (!this->collect_thread.joinable()) {
        this->collect_thread = std::thread([&](collector *gc) { gc->collect_loop(); }, this);
    }
}

void collector::collect_loop() {
    while (!this->stop_collect_thread) {
        unsigned long freed_count = this->collect();

        /* if we didn't free anything, sleep for a set interval */
        if (this->sweep_wait_time.count() > 0 && freed_count == 0) {
            std::this_thread::sleep_for(this->sweep_wait_time);
        }
    }
}

void collector::finalize(i_managed *object) {
    std::lock_guard lock(this->object_map_mutex);

    delete object;
    this->objects.erase(object);

    for (auto object_ptr : this->objects) {
        if (object_ptr->references.contains(object)) {
            object_ptr->references.erase(object);
        }
    }
}

std::chrono::milliseconds collector::get_sweep_wait_time() {
    return this->sweep_wait_time;
}

void collector::remove_reference(i_managed &object) {
    std::scoped_lock lock(this->object_map_mutex);

    /* decrement the refcount and finalize this object as necessary */
    if (--object.refcount == 1) {
        this->finalize(&object);
    }
}

void collector::set_sweep_wait_time(std::chrono::milliseconds wait_time) {
    this->sweep_wait_time = wait_time;
}

unsigned long collector::sweep() {
    std::scoped_lock lock(this->object_map_mutex);

    /* perform a depth-first search over the graph of all known objects */
    i_managed_set targets(this->objects.begin(), this->objects.end());
    i_managed_set visited;

    phmap::parallel_flat_hash_map<i_managed *, unsigned int> ref_counts;

    while (!targets.empty()) {
        /* get the current root node */
        i_managed *current_node = *targets.begin();

        /* increment the cyclic ref count of the node */
        if (!ref_counts.contains(current_node)) {
            ref_counts.insert_or_assign(current_node, 0);
        }
        ref_counts.at(current_node)++;

        /* loop over all child references of the current node */
        for (auto child_node : current_node->references) {
            /* if we haven't visited this child, make it a target */
            if (!visited.contains(child_node)) {
                targets.insert(child_node);
            }

            /* regardless of if it's a new target, increment the child node refcount */
            if (!ref_counts.contains(child_node)) {
                ref_counts.insert_or_assign(child_node, 0);
            }
            ref_counts.at(child_node)++;
        }

        /* remove the current node from the list of targets and mark it visited */
        targets.erase(current_node);
        visited.insert(current_node);
    }

    /* loop over all of the objects we saw. if there are no external references to any
        objects, then they are only referenced cyclically. delete them in that case */
    unsigned long finalized_count = 0;
    for (auto pair : ref_counts) {
        /* the reference is cyclic if the refcount that we observed was the same as the
            objects actual refcount. we ignore objects with an observed refcount of 1
            as these will be freed by the refcount finalization */
        if (pair.second != 1 && pair.first->refcount == pair.second) {
            this->finalize(pair.first);
            finalized_count++;
        }
    }

    return finalized_count;
}
