FROM gcc:latest
# Install GNU Debugger
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    # GNU Debugger
    gdb \
    clang-tidy

WORKDIR /code
