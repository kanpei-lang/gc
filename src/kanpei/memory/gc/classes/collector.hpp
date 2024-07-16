#ifndef __KANPEI_MEMORY_GC_COLLECTOR
#define __KANPEI_MEMORY_GC_COLLECTOR

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "i_managed.hpp"
#include "i_managed_set.hpp"
#include "object_state.hpp"

namespace kanpei {
    namespace memory {
        namespace gc {
            template <typename>
            class ref;

            template <typename>
            class managed_ptr;

            class collector {
               private:
                std::thread collect_thread;
                std::atomic<bool> stop_collect_thread = false;
                std::chrono::duration<int64_t, std::milli> sweep_wait_time =
                    std::chrono::milliseconds(10);

                i_managed_set objects;

                void collect_loop();
                void finalize(i_managed *object);
                unsigned long sweep();

               public:
                std::recursive_mutex object_map_mutex;

                ~collector();

                template <typename T>
                ref<managed_ptr<T>> allocate() {
                    return ref<managed_ptr<T>>(
                        new managed_ptr<T>((T *)::operator new(sizeof(T)), this)
                    );
                }

                template <typename T, std::size_t size>
                ref<managed_ptr<T[size]>> allocate() {
                    return ref<managed_ptr<T[size]>>(
                        new managed_ptr<T[size]>((T[size])::operator new(sizeof(T) * size), this)
                    );
                }

                void add_reference(i_managed &object);
                unsigned long collect();
                void collect_forever();
                std::chrono::milliseconds get_sweep_wait_time();
                void set_sweep_wait_time(std::chrono::milliseconds wait_time);
                void remove_reference(i_managed &object);
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_COLLECTOR */