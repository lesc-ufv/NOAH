FROM ubuntu:16.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc g++ \
    python python-dev \
    pkg-config sqlite3 libsqlite3-dev \
    qt5-default \
    mercurial git wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN wget https://www.nsnam.org/release/ns-allinone-3.25.tar.bz2 && \
    tar xjf ns-allinone-3.25.tar.bz2 && \
    cd ns-allinone-3.25 && \
    ./build.py --enable-examples --enable-tests

WORKDIR /opt/ns-allinone-3.25/ns-3.25

CMD ["/bin/bash"]
