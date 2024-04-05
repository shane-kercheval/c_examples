FROM gcc:latest
# Install GNU Debugger
RUN apt-get update && apt-get install -y gdb
WORKDIR /code
