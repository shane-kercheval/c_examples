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
run:
	cc ./examples/pointers.c -o ./examples/pointers.out
	./examples/pointers.out

run_no_debug:  # turns off debug mode and assertion statements are not executed
	cc -DNDEBUG ./examples/pointers.c -o ./examples/pointers.out
	./examples/pointers.out


linting:
	ruff check source/config

tests:
	pytest tests
