# Arguments for producer_consumer executable
NUM_PRODUCERS ?= 2
NUM_CONSUMERS ?= 2
BUFFER_SIZE ?= 10
MAX_REQUESTS ?= 10000

# by default the build type is Debug
# in order to run the executable with the Release build type, run `make run BUILD_TYPE=Release`
# see CMakeLists.txt for exact compiler flags for each build type
# For example, assert statements (e.g. used in utils.c for locking macros) are only enabled in Debug
# build type but are optimized out for Release build type
BUILD_TYPE ?= Debug
BUILD_DIR := build

.PHONY: all debug release compile setup_build_dir clean tests run

all: compile tests

debug: BUILD_TYPE := Debug
debug: compile tests

release: BUILD_TYPE := Release
release: compile tests

# Create the build directory and run cmake
setup_build_dir:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

# Compile the project using the generated Makefile by CMake
compile: setup_build_dir
	$(MAKE) -C $(BUILD_DIR)

# Run tests
tests: compile
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

run: compile
	@$(BUILD_DIR)/producer_consumer $(NUM_PRODUCERS) $(NUM_CONSUMERS) $(BUFFER_SIZE) $(MAX_REQUESTS) || { echo 'Error running producer_consumer'; exit 1; }

# check for concurrency errors
check_concurrency: BUILD_TYPE := Release
check_concurrency: compile
	valgrind --tool=helgrind -s $(BUILD_DIR)/producer_consumer $(NUM_PRODUCERS) $(NUM_CONSUMERS) $(BUFFER_SIZE) $(MAX_REQUESTS)

# check for memory leaks
check_memory: BUILD_TYPE := Release
check_memory: compile
	valgrind --leak-check=full --track-origins=yes -s $(BUILD_DIR)/producer_consumer $(NUM_PRODUCERS) $(NUM_CONSUMERS) $(BUFFER_SIZE) $(MAX_REQUESTS)
