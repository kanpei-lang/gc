#ifndef __GC_CLASSES_MANAGED_PTR_DEF
#define __GC_CLASSES_MANAGED_PTR_DEF

namespace kanpei {
    namespace memory {
        class garbage_collector;

        template <typename T>
        class managed_ptr {
           private:
            garbage_collector *parent;
            T *object;

           public:
            managed_ptr<T>(T *object, garbage_collector *parent);
            managed_ptr<T>(const managed_ptr<T> &other);
            ~managed_ptr();

            T &operator*();
            managed_ptr<T> &operator=(const managed_ptr<T> &other);
        };
    }  // namespace memory
}  // namespace kanpei

#endif  // __GC_CLASSES_MANAGED_PTR_DEf