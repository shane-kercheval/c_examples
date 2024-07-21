BUILD_DIR := build

CLIENT_EXEC := client
SERVER_EXEC := server
TEST_EXEC := test_client test_server test_utils

.PHONY: all debug release compile setup_build_dir clean tests run

all: compile tests

debug: BUILD_TYPE := Debug
debug: compile tests

release: BUILD_TYPE := Release
release: compile tests

setup_build_dir:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..

compile: setup_build_dir
	$(MAKE) -C $(BUILD_DIR)

tests: compile
	cd $(BUILD_DIR) && ctest --output-on-failure

tests_memory: BUILD_TYPE := Release
tests_memory: compile
	valgrind --leak-check=full --track-origins=yes $(BUILD_DIR)/tests/test_protocol

clean:
	rm -rf $(BUILD_DIR)

run_client: compile
	@$(BUILD_DIR)/src/$(CLIENT_EXEC) || { echo 'Error running client'; exit 1; }

run_server: compile
	@$(BUILD_DIR)/src/$(SERVER_EXEC) || { echo 'Error running server'; exit 1; }
