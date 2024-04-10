####
# DOCKER
####
docker_build:
	docker compose -f docker-compose.yml build

docker_run: docker_build
	docker compose -f docker-compose.yml up

docker_down:
	docker compose down --remove-orphans

docker_rebuild:
	docker compose -f docker-compose.yml build --no-cache

docker_bash:
	docker compose -f docker-compose.yml up --build bash


####
# Project
####
run_c:
	cc ./c_examples/types.c -o ./c_examples/a.out
	./c_examples/a.out

run_c_no_debug:  # turns off debug mode and assertion statements are not executed
	cc -DNDEBUG ./c_examples/types.c -o ./c_examples/a.out
	./c_examples/a.out

run_cpp_17:
	g++ -o c++_examples/output_cpp c++_examples/check_cpp_standard.cpp
	./c++_examples/output_cpp

run_cpp_20:
	g++ -std=c++20 -o c++_examples/output_cpp c++_examples/check_cpp_standard.cpp
	./c++_examples/output_cpp
