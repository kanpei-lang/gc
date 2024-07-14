#ifndef __KANPEI_MEMORY_GC_MANAGED_OBJECT
#define __KANPEI_MEMORY_GC_MANAGED_OBJECT

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "object_state.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            class managed_object {
               private:
                phmap::parallel_flat_hash_map<void *, object_state> references;

               public:
                managed_object();
                ~managed_object();
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif  // __KANPEI_MEMORY_GC_MANAGED_OBJECT