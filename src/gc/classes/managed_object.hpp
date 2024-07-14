#ifndef __GC_CLASSES_MANAGED_OBJECT
#define __GC_CLASSES_MANAGED_OBJECT

#include "../parallel-hashmap/parallel_hashmap/phmap.h"

namespace kanpei {
    namespace memory {
        class managed_object {
           private:
            phmap::parallel_flat_hash_map<void *, unsigned int> references;

           public:
            managed_object();
            ~managed_object();
        };
    }  // namespace memory
}  // namespace kanpei

#endif  // __GC_CLASSES_MANAGED_OBJECT