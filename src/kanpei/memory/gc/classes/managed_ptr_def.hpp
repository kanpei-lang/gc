#ifndef __KANPEI_MEMORY_GC_MANAGED_PTR_DEF
#define __KANPEI_MEMORY_GC_MANAGED_PTR_DEF

#include "i_managed.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            class collector;

            template <typename T>
            class managed_ptr : public i_managed {
                T *object;

               public:
                managed_ptr<T>();
                managed_ptr<T>(T *object, collector *parent);
                managed_ptr<T>(const managed_ptr<T> &other);
                ~managed_ptr();

                T &operator*();
                managed_ptr<T> &operator=(const managed_ptr<T> &other);
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_MANAGED_PTR_DEF */