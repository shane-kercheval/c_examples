# by default the build type is Release
# in order to run the executable with the debug build type, run `make run BUILD_TYPE=Debug`
# see CMakeLists.txt for exact compiler flags for each build type
BUILD_TYPE ?= Release
BUILD_DIR := build

.PHONY: all debug release compile setup_build_dir clean tests run

all: compile tests

debug: BUILD_TYPE := Debug
debug: compile tests

release: BUILD_TYPE := Release
release: compile

# Create the build directory and run cmake
setup_build_dir:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

# Compile the project using the generated Makefile by CMake
compile: setup_build_dir
	$(MAKE) -C $(BUILD_DIR)

# Always run tests in Debug mode
# this is needed so we don't optimize out the test code (e.g. assert statements)
tests: BUILD_TYPE := Debug
tests: compile
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

run: compile
	@$(BUILD_DIR)/producer_consumer || { echo 'Error running producer_consumer'; exit 1; }
