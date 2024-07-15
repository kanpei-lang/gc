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

void collector::collect() {
    unsigned long freed_count = this->sweep();

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

void collector::finalize(i_managed *object) {
    std::lock_guard lock(this->object_map_mutex);

    delete object;
    this->objects.erase(object);
}

void collector::remove_reference(i_managed &object) {
    std::scoped_lock lock(this->object_map_mutex);

    /* decrement the refcount and finalize this object as necessary */
    if (--object.refcount == 0) {
        this->finalize(&object);
    }
}

unsigned long collector::sweep() {
    std::scoped_lock lock(this->object_map_mutex);

    /* perform a depth-first search over the graph of all known objects */
    i_managed_set targets(this->objects.begin(), this->objects.end());
    i_managed_set visited;

    phmap::parallel_flat_hash_map<i_managed *, int> ref_counts;

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

    /* loop over all of the objects we saw. if there are no external references any
        objects, then they are only referenced cyclically. delete them in that case */
    unsigned long finalized_count = 0;
    for (auto pair : ref_counts) {
        if (pair.first->refcount == pair.second) {
            this->finalize(pair.first);
            finalized_count++;
        }
    }

    return finalized_count;
}
