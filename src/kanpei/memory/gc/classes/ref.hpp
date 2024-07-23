#ifndef __KANPEI_MEMORY_GC_REF
#define __KANPEI_MEMORY_GC_REF

#include <cxxabi.h>

#include <iostream>

#include "collector.hpp"
#include "i_managed.hpp"
#include "managed_ptr.hpp"

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
                    /* remove reference to the current child object */
                    this->object->refcount--;
                    this->object->parent->remove_reference(*this->object);

                    /* add a reference to the object in the other ref<T> */
                    this->object = other.object;
                    this->object->refcount++;
                    other.object->parent->add_reference(*(other.object));

                    return *this;
                }

                template <typename U = T>
                typename std::enable_if<is_managed_ptr<U>::value, typename U::pointer_type>::type &
                operator*() const {
                    return **(T *)this->object;
                }

                template <typename U = T>
                typename std::enable_if<!is_managed_ptr<U>::value, U>::type &operator*() const {
                    return *(T *)this->object;
                }

                T *operator->() const {
                    return &(**this);
                }

                T *get_pointer() {
                    return (T *)this->object;
                }

                friend std::ostream &operator<<(std::ostream &out, const ref &obj) {
                    /* derive the demangled name from the template type */
                    int status;
                    char *demangled_name_buf =
                        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);

                    /* remove any namespace prefixes from the demangled name */
                    std::string demangled_name(demangled_name_buf);
                    std::size_t last_delimiter_pos = demangled_name.rfind("::");
                    if (last_delimiter_pos != std::string::npos) {
                        demangled_name = demangled_name.substr(last_delimiter_pos + 2);
                    }

                    out << "ref<" << demangled_name << "> @ " << &obj
                        << ", refcount=" << obj.object->refcount << ", object=" << obj.object;

                    /* free the demangled name buffer */
                    free(demangled_name_buf);

                    return out;
                }
            };
        }  // namespace gc
    }  // namespace memory
}  // namespace kanpei

#endif /* __KANPEI_MEMORY_GC_REF */