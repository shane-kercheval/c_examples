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
	cc ./examples/hello_world.c -o ./examples/hello_world.out
	./examples/hello_world.out


linting:
	ruff check source/config

tests:
	pytest tests
