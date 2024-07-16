FROM gcc:latest

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    software-properties-common \
    man-db \
    zsh \
    lsof \
    gcc \
    gdb \
    make \
    cmake \
    curl \
    valgrind

# Set default shell to zsh
RUN echo "dash dash/sh boolean false" | debconf-set-selections && dpkg-reconfigure dash

# Clean up to reduce image size
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /code
