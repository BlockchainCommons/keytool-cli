FROM debian:latest as bc-base
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update -y 
RUN apt install -y  \
    build-essential autogen libtool shtool pkg-config autoconf-archive     \
    clang llvm lld libstdc++6 libc++-dev libc++1 libc++abi1         \
    python \
    git-buildpackage    \
    git                 \
    coreutils binutils          \
    shunit2     \
    bash

FROM bc-base
ARG DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]
WORKDIR /app
COPY . source
COPY .git source/.git
WORKDIR source

RUN ln -svf /usr/bin/lld /usr/bin/ld
RUN git submodule update
RUN cat debian/patches/series | xargs -I{} patch -f --strip=1 -i "debian/patches/{}" || true
RUN gbp buildpackage --git-upstream-tree=master --git-debian-branch=debian/latest --git-ignore-new  --git-force-create --git-submodules