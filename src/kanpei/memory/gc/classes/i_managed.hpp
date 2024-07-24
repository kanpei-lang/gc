#ifndef __KANPEI_MEMORY_GC_I_MANAGED
#define __KANPEI_MEMORY_GC_I_MANAGED

namespace kanpei {
    namespace memory {
        namespace gc {
            class i_managed;
        }
    }  // namespace memory
}  // namespace kanpei

#include "i_managed_set.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            class collector;

            class i_managed {
               public:
                bool is_fundamental;
                unsigned int refcount = 0;
                collector *parent;
                i_managed_set references;

                i_managed(bool is_fundamental = true) : is_fundamental(is_fundamental) {}
                virtual ~i_managed() {}

                bool operator==(const i_managed &other) const {
                    return this == &other;
                }
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

namespace std {
    template <>
    struct hash<kanpei::memory::gc::i_managed> {
        std::size_t operator()(const kanpei::memory::gc::i_managed &obj) const {
            return (std::size_t)&obj;
        }
    };
}  // namespace std

#endif /* __KANPEI_MEMORY_GC_I_MANAGED */