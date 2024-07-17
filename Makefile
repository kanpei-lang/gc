CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -pthread
TESTCXXFLAGS := -I/usr/include/gtest
LDFLAGS = -lgtest -lgtest_main
COVFLAGS = -fprofile-arcs -ftest-coverage

BUILDDIR := ./build
TESTEXECUTABLE := gc_tests
TESTTARGET := $(BUILDDIR)/$(TESTEXECUTABLE)

LIBDIR := ./lib
LIBTARGET := $(LIBDIR)/libkanpei_gc.so

COVDIR := ./cov
COVPERCENT := 60

INSTALLINCLUDEDIR := /usr/include
INSTALLLIBDIR := /usr/lib

all: build_lib

clean:
	rm -rf $(COVDIR)
	rm -rf $(BUILDDIR)
	rm -rf $(LIBDIR)

build_lib: ensure_lib_dir
	$(CXX) $(CXXFLAGS) -fPIC ./src/kanpei/memory/gc/classes/*.cpp -c
	mv *.o $(LIBDIR)
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $(LIBTARGET) ./src/kanpei/memory/gc/classes/*.cpp

coverage:
	mkdir -p $(COVDIR)
	gcov $(BUILDDIR)/*.gcno > /dev/null
	mv ./*.gcov $(COVDIR)
	python tests/analyze_coverage.py $(COVDIR) $(COVPERCENT)

ensure_build_dir:
	mkdir -p $(BUILDDIR)

ensure_lib_dir:
	mkdir -p $(LIBDIR)

install:
	cp -r src/kanpei $(INSTALLINCLUDEDIR)
	cp $(LIBTARGET) $(INSTALLLIBDIR)

build_tests: ensure_build_dir
	$(CXX) $(CXXFLAGS) $(TESTCXXFLAGS) $(COVFLAGS) -o $(TESTTARGET) ./src/kanpei/memory/gc/classes/*.cpp ./tests/*.cpp $(LDFLAGS)

run_tests:
	$(TESTTARGET)

tests: build_tests run_tests coverage

.PHONY: all build_lib build_tests clean coverage tests run_tests