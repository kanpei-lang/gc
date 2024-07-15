#ifndef __KANPEI_MEMORY_GC_COLLECTOR
#define __KANPEI_MEMORY_GC_COLLECTOR

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "i_managed.hpp"
#include "i_managed_set.hpp"
#include "managed_ptr.hpp"
#include "object_state.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            template <typename>
            class ref;

            class collector {
               private:
                std::thread collect_thread;
                std::atomic<bool> stop_collect_thread = false;

                i_managed_set objects;
                std::recursive_mutex object_map_mutex;

                void collect_loop();
                void finalize(i_managed *object);
                unsigned long sweep();

               public:
                ~collector();

                template <typename T>
                ref<managed_ptr<T>> allocate() {
                    return ref<managed_ptr<T>>(new managed_ptr<T>((T *)malloc(sizeof(T)), this));
                }

                template <typename T, std::size_t size>
                ref<managed_ptr<T[size]>> allocate() {
                    return ref<managed_ptr<T[size]>>(
                        new managed_ptr<T[size]>((T[size])malloc(sizeof(T) * size), this)
                    );
                }

                void add_reference(i_managed &object);
                void collect();
                void collect_forever();
                void remove_reference(i_managed &object);
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_COLLECTOR */