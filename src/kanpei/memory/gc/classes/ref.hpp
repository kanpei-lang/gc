#ifndef __KANPEI_MEMORY_GC_REF
#define __KANPEI_MEMORY_GC_REF

#include "collector.hpp"
#include "i_managed.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            template <typename T>
            class ref {
               private:
                i_managed *object;

               public:
                ref() {
                    this->object = nullptr;
                }

                ref(i_managed *object) {
                    this->object = object;
                    this->object->refcount++;
                    object->parent->add_reference(*object);
                }

                ref(const ref<T> &other) {
                    this->object = other.object;
                    this->object->refcount++;
                    other.object->parent->add_reference(*(other.object));
                }

                ~ref() {
                    this->object->refcount--;

                    /* NOTE: the parent can free this->object if its refcount gets
                        down to 1 in remove_reference(). DO NOT use this->object after
                        this function call */
                    this->object->parent->remove_reference(*(this->object));
                }

                ref<T> &operator=(const ref<T> &other) {
                    this->object = other.object;
                    this->object->refcount++;
                    other.object->parent->add_reference(*(other.object));

                    return *this;
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