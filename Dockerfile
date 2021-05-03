FROM ubuntu:20.04 as build-stage
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y
# Install build dependencies
RUN apt-get install -y lsb-release wget software-properties-common git apt-transport-https 
RUN apt-get install -y build-essential pkg-config autoconf libtool shunit2 libc++-10-dev libc++abi-10-dev python python-dev
RUN apt-get -y install 
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x llvm.sh
RUN ./llvm.sh 10  # version 10
# Build keytool-cli
COPY . /keytool-cli
WORKDIR /keytool-cli
ENV CC="clang-10" CXX="clang++-10"
ENV WALLY_CONFIGURE="--disable-clear-tests"
RUN apt-get install -y python3 python3-dev
RUN ./build.sh

# Export built executable to a minimal runtime image and run as an unprivileged
# user.
FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
# Install runtime dependencies
RUN apt-get update -y
RUN apt-get install -y libc++1 libc++abi1
# Install binary
COPY --from=build-stage /keytool-cli/src/keytool /usr/local/bin/keytool
# Create an unprivileged user
RUN useradd --create-home --user-group user
USER user
ENTRYPOINT ["keytool"]
