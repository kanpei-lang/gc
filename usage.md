# usage.md

## Allocating primitives
**WRITE A PREAMBLE HERE!!**

For example, here is a very simple example program that allocates a single managed `int` and assigns a value to it:

```c++
#include <kanpei/memory/gc/all.hpp>

using namespace kanpei::memory::gc;

/* global garbage collector instance */
collector gc;

int main() {
    /* start the garbage collection thread */
    gc->collect_forever();

    /* allocate a managed int and assign the value 123 to it */
    ref<managed_ptr<int>> example_int = gc.allocate<int>(123);

    return 0;
}
```

