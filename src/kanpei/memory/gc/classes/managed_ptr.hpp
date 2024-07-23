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
                using pointer_type = T;

                managed_ptr();
                managed_ptr(T *object, collector *parent);
                managed_ptr(const managed_ptr<T> &other);
                ~managed_ptr();

                T &operator*();
                managed_ptr<T> &operator=(const managed_ptr<T> &other);
            };

            template <typename T>
            struct is_managed_ptr {
                static const bool value = false;
            };

            template <typename T>
            struct is_managed_ptr<managed_ptr<T>> {
                static const bool value = true;
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

/* include the implementation file for templated member functions */
#include "managed_ptr.cpp"

#endif /* __KANPEI_MEMORY_GC_MANAGED_PTR_DEF */