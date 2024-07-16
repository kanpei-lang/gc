CXX := g++
CXXFLAGS = -Wall -Wextra -std=c++20 -I/usr/include/gtest -pthread
LDFLAGS = -lgtest -lgtest_main -pthread
COVFLAGS = -fprofile-arcs -ftest-coverage

BUILDDIR := ./build
TESTEXECUTABLE := gc_tests
TESTTARGET := $(BUILDDIR)/$(TESTEXECUTABLE)

COVDIR := ./cov
COVPERCENT := 55

all: clean tests coverage

clean:
	rm -rf $(COVDIR)
	rm -rf $(BUILDDIR)

coverage:
	mkdir -p $(COVDIR)
	gcov $(BUILDDIR)/*.gcno > /dev/null
	mv ./*.gcov $(COVDIR)
	python tests/analyze_coverage.py $(COVDIR) $(COVPERCENT)

ensure_build_dir:
	mkdir -p $(BUILDDIR)

build_tests: ensure_build_dir
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -o $(TESTTARGET) ./src/kanpei/memory/gc/classes/*.cpp ./tests/*.cpp $(LDFLAGS)

run_tests:
	$(TESTTARGET)

tests: build_tests run_tests

.PHONY: all build_tests clean coverage tests run_tests