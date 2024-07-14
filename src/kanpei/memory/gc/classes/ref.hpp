#ifndef __KANPEI_MEMORY_GC_REF
#define __KANPEI_MEMORY_GC_REF

#include "i_managed.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            template <typename T>
            class ref {
               private:
                i_managed *object;

               public:
                ref<T>(i_managed *object) {
                    this->object = object;
                    object->parent->add_reference(*object);
                }

                ref<T>(const ref<T> &other) {
                    this->object = other.object;
                    other.object->parent->add_reference(*(other.object));
                }

                ~ref<T>() {
                    this->object->parent->remove_reference(*(this->object));
                }

                ref<T> &operator=(const ref<T> &other) {
                    this->object = other.object;
                    other.object->parent->add_reference(*(other.object));
                }

                T &operator*() {
                    return *(T *)this->object;
                }

                T *operator->() const {
                    return (T *)this->object;
                }
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_REF */