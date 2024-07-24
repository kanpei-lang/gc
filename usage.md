# usage.md

## Installation
To integrate with the Kanpei garbage collector, you will need to build and install the relevant dynamic library. First, clone this repository:

```shell
git clone https://github.com/kanpei-lang/gc.git
cd gc
```

Then, use the default and install targets of the `Makefile` to install locally:

```shell
make
sudo make install
```

## Working with fundamental types
### Allocating managed instances
The Kanpei garbage collector supports managed instances of fundamental C++ types like `int`, `long`, and `bool` via references that wrap the `managed_ptr<T>` class. The most simple way to create a managed fundamental object is using the `collector::allocate<T>()` method.

As an example, here is a very simple example program that allocates a single managed `int` and assigns a value to it:

```c++
#include <iostream>
#include <kanpei/memory/gc/all.hpp>

using namespace kanpei::memory::gc;

/* global garbage collector instance */
collector gc;

int main() {
    /* start the garbage collection thread */
    gc.collect_forever();

    /* allocate a managed int and assign the value 123 to it */
    ref<managed_ptr<int>> example_int = gc.allocate<int>(123);

    /* outputs '123' to the console

        note that ref<T> is templated to automatically unwrap the managed_ptr<T>
        and deference the object instance using the '*' operator */
    std::cout << *example_int << std::endl;

    return 0;
}
```

### Managing pointers
It is also possible to use the garbage collector to manage pointers to fundamental objects that were allocated elsewhere. Here is an example of a C function that creates a char buffer with `malloc()` and returns it. Note that when wrapping this in a `managed_ptr<char>`, we need to provide a reference to `free()` as the appropriate deleter function since the buffer was allocated with `malloc()`.

```c++
#include <iostream>
#include <kanpei/memory/gcc/all.hpp>

/* global garbage collector instance */
collector gc;

/* example function that creates and returns a buffer with malloc() */
char *example_func() {
    char *buffer = (char*)malloc(13);
    strcpy(buffer, "hello, world");

    return buffer;
}

int main() {
    /* start the garbage collection thread */
    gc.collect_forever();

    /* allocate the buffer using the function */
    char *buffer = example_func();

    /* wrap the buffer in a managed_ptr<char> so that it will
        be automatically managed for us
    
        note that we have to provide free() as the deleter since
        the buffer was created with malloc() */
    ref<managed_ptr<char>> example_buffer(
        new managed_ptr<char>(buffer, &gc, &free)
    );

    /* outputs 'hello, world' */
    std::cout << &*example << std::endl;

    return 0;
}
```
