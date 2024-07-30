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
# which compiler is cc using? `readlink -f $(which cc)`
# which version is cc using? `cc --version`
# which version of c standard is cc using? `echo | cc -dM -E - | grep __STDC_VERSION__`
# The output is `#define __STDC_VERSION__ 201710L` which means the compiler is using C17 standard

run_http_server:
	cc ./socket_tutorial/http_server.c -o ./socket_tutorial/server
	./socket_tutorial/server

run_http_client:
	cc ./socket_tutorial/http_client.c -o ./socket_tutorial/client
	./socket_tutorial/client localhost 3490
	# ./socket_tutorial/client 93.184.215.14 80
	# ./socket_tutorial/client 142.250.72.174 80

run_socket_server:
	cc ./socket_tutorial/tcp_server.c -o ./socket_tutorial/server
	./socket_tutorial/server

run_socket_client:
	cc ./socket_tutorial/tcp_client.c -o ./socket_tutorial/client
	./socket_tutorial/client

run_beej_server:
	cc ./beej/server.c -o ./beej/server
	./beej/server

run_beej_client:
	cc ./beej/client.c -o ./beej/client
	./beej/client localhost

run_showip:
	cc ./beej/showip.c -o ./beej/showip
	./beej/showip example.com

run_c_types:
	cc ./c_examples/types.c -o ./c_examples/a.out
	./c_examples/a.out

run_c_pointers:
	# clang-tidy ./c_examples/pointers.c --
	# compile with C Standard 23
	cc -std=c23 ./c_examples/pointers.c -o ./c_examples/a.out
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

run_ref:
	g++ -std=c++20 -fmodules-ts -o c++_examples/output_cpp c++_examples/references.cpp
	./c++_examples/output_cpp

run_classes:
	g++ -std=c++20 -fmodules-ts -o c++_examples/output_cpp c++_examples/classes.cpp
	./c++_examples/output_cpp

run_modules:
	g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
	g++ -std=c++20 -fmodules-ts -c c++_examples/modules.cpp -o modules.o
	g++ -std=c++20 -fmodules-ts c++_examples/modules_user.cpp modules.o -o c++_examples/output_cpp 
	./c++_examples/output_cpp

run_vector:
	# g++ \
	# 	-Ic++_examples/vector \
	# 	-o c++_examples/vector_user \
	# 	c++_examples/vector/vector.cpp \
		# c++_examples/vector_user.cpp
	g++ \
		-std=c++20 \
		-Ic++_examples/vector \
		-o c++_examples/output_cpp \
		c++_examples/vector/vector.cpp c++_examples/vector_user.cpp
	./c++_examples/output_cpp
