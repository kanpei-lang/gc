#ifndef __GC_CLASSES_GARBAGECOLLECTOR
#define __GC_CLASSES_GARBAGECOLLECTOR

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "managed_ptr.hpp"

namespace kanpei {
    namespace memory {
        class garbage_collector {
           private:
            std::thread collect_thread;
            std::atomic<bool> stop_collect_thread = false;

            phmap::parallel_flat_hash_map<void *, unsigned int> objects;
            std::mutex object_map_mutex;

            void collect_loop();

           public:
            ~garbage_collector();

            template <typename T>
            managed_ptr<T> allocate() {
                return managed_ptr<T>((T *)malloc(sizeof(T)), this);
            }

            template <typename T, size_t size>
            managed_ptr<T[size]> allocate() {
                return managed_ptr<T[]>((T *)malloc(sizeof(T) * size), this);
            }

            void add_reference(void *object);
            void collect();
            void collect_forever();
            void remove_reference(void *object);
        };
    }  // namespace memory
}  // namespace kanpei

#endif  // __GC_CLASSES_GARBAGECOLLECTOR