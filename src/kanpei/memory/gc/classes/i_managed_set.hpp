#ifndef __KANPEI_MEMORY_GC_I_MANAGED_SET
#define __KANPEI_MEMORY_GC_I_MANAGED_SET

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "i_managed.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            typedef phmap::parallel_flat_hash_set<kanpei::memory::gc::i_managed *> i_managed_set;
        }
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_I_MANAGED_SET */