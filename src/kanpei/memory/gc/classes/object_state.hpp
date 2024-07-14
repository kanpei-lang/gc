#ifndef __KANPEI_MEMORY_GC_OBJECT_STATE
#define __KANPEI_MEMORY_GC_OBJECT_STATE

namespace kanpei {
    namespace memory {
        namespace gc {
            struct object_state {
                int refcount;
                bool is_primitive;
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif  // __KANPEI_MEMORY_GC_OBJECT_STATE