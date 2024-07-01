
#
#	Makefile options
#

NUM_PROCESSORS				:= $(shell nproc)
MAKEFLAGS 					+= -j$(NUM_PROCESSORS)
CMAKE_BUILD_TYPE			?= Release
CMAKE_TRACE_OPTIONS			?= #--trace --trace-expand --debug-output --warn-uninitialized --warn-unused-vars
CMAKE_BUILD_OPTIONS			?=
CMAKE_POSITIONAL_ARGS		?= -j$(NUM_PROCESSORS) VERBOSE=1

#
#	Extensions
#

CPP							:= .cc
TPP							:= .tpp
HEADERS						:= .h

#
#	Main targets
#

define build
	cmake -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) -B build $(CMAKE_TRACE_OPTIONS) $(CMAKE_BUILD_OPTIONS); \
	cmake --build build --target $(1) -- $(CMAKE_POSITIONAL_ARGS);
endef

all: app

app:
	$(call build,Transactions)
	./build/Transactions

test:
	$(call build,tests)
	ctest --test-dir build --build-target tests --output-on-failure -j$(NUM_PROCESSORS)

clean:
	rm -rf build

#
#	Format targets
#

FORMAT_FILE_PATTERN			:= -iname "*$(CPP)" -o -iname "*$(HEADERS)" -o -iname "*$(TPP)"
#CLANG_FORMAT_OPTIONS		:= --style=google $(1) --verbose
clang_format				= find . $(FORMAT_FILE_PATTERN) | xargs clang-format --style=google $(1) --verbose

format_set:
	$(call clang_format,-i)

format_check:
	$(call clang_format,-n)

#
#	Other
#

.PHONY:
	all
	app
	test
	coverage
	open_coverage_report
	valgrind
	clean
	format_set
	format_check
