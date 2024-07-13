#ifndef __GC_CLASSES_GARBAGECOLLECTOR
#define __GC_CLASSES_GARBAGECOLLECTOR

#include <cstdlib>

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "managed_ptr.hpp"

namespace kanpei {
    namespace memory {
        class garbage_collector {
           private:
            phmap::parallel_flat_hash_map<void *, unsigned int> objects;

           public:
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

            void remove_reference(void *object);
        };
    }  // namespace memory
}  // namespace kanpei

#endif  // __GC_CLASSES_GARBAGECOLLECTOR