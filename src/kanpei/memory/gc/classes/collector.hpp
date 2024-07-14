#ifndef __KANPEI_MEMORY_GC_COLLECTOR
#define __KANPEI_MEMORY_GC_COLLECTOR

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "managed_object.hpp"
#include "managed_ptr.hpp"
#include "object_state.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            class collector {
               private:
                std::thread collect_thread;
                std::atomic<bool> stop_collect_thread = false;

                phmap::parallel_flat_hash_map<void *, object_state> objects;
                std::mutex object_map_mutex;

                void collect_loop();
                unsigned long mark();

               public:
                ~collector();

                template <typename T>
                managed_ptr<T> allocate() {
                    return managed_ptr<T>((T *)malloc(sizeof(T)), this);
                }

                template <typename T, size_t size>
                managed_ptr<T[size]> allocate() {
                    return managed_ptr<T[size]>((T[size])malloc(sizeof(T) * size), this);
                }

                template <typename T>
                void add_reference(T *object, bool is_primitive) {
                    std::scoped_lock lock(this->object_map_mutex);

                    if (!this->objects.contains(object)) {
                        this->objects.insert_or_assign(object, object_state{0, is_primitive});
                    }

                    this->objects.at(object).refcount += 1;
                }

                void collect();
                void collect_forever();

                template <typename T>
                void remove_reference(T *object) {
                    std::scoped_lock lock(this->object_map_mutex);

                    this->objects.at(object).refcount -= 1;
                }
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif  // __KANPEI_MEMORY_GC_COLLECTOR