#ifndef __KANPEI_MEMORY_GC_MANAGED_OBJECT
#define __KANPEI_MEMORY_GC_MANAGED_OBJECT

#include "../parallel-hashmap/parallel_hashmap/phmap.h"
#include "collector.hpp"
#include "i_managed.hpp"
#include "object_state.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            class managed_object : public i_managed {
               public:
                managed_object(collector *parent);
                ~managed_object();

                void add_reference(i_managed &object);
                void add_reference(i_managed *object);

                template <typename T>
                void add_reference(ref<T> &object) {
                    if (this->references.contains(&*object)) {
                        return;
                    }

                    this->references.insert(&*object);
                    this->parent->add_reference(*object);
                }

                void remove_reference(i_managed &object);
                void remove_reference(i_managed *object);

                template <typename T>
                void remove_reference(ref<T> &object) {
                    if (!this->references.contains(&*object)) {
                        return;
                    }

                    this->references.erase(&*object);
                    this->parent->remove_reference(*object);
                }
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_MANAGED_OBJECT */