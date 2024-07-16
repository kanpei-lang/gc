# 🟠 kanpei-lang/gc
<i> A garbage collector for the Kanpei programming language</i>
<hr />

## Introduction
This repo contains the garbage collector that is used by the reference Kanpei interpreter. It supports automatic management of heap-allocated primitives and user-defined classes.

Collection is performed using a combination of reference counting and tracing. Cyclic references are detected and freed by identifying isolated subgraphs within the known graph of managed objects.

## Cloning
This repository include submodules. Make sure to include submodules by using the following command:

```shell
git clone --recurse-submodules https://github.com/kanpei-lang/gc.git
```

## Running tests
The `Makefile` bundled with this repository be used to build and run the garbage collector unit tests and automatically evaluate test coverage.

In order to run the tests, you will need to have [GoogleTest](https://github.com/google/googletest) and [gcov](https://gcc.gnu.org/onlinedocs/gcc/gcov/introduction-to-gcov.html) installed along with the [gcc compiler](https://gcc.gnu.org/) and [make](https://www.gnu.org/software/make/). If you are running Arch Linux or a derivate distribution, you can install these requirements by running the following:

```shell
pacman -S make gcc gtest
```

Once you have installed the dependencies, build and run the tests by running `make` with the default target:

```shell
make
```

## Acknowledgements
This garbage collector depends on the `phmap::parallel_flat_hash_set` container class from [greg7mdp/parallel-hashmap](https://github.com/greg7mdp/parallel-hashmap). The source for the `parallel_flat_hash_set` is not bundled in this repository but is cloned as a submodule. You may review the license for the `parallel-hashmap` repo [here](https://github.com/greg7mdp/parallel-hashmap/blob/master/LICENSE).
