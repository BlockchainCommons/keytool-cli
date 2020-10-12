#!/bin/bash

set -e

git submodule init
git submodule update

source set_build_paths.sh

# Terminal colors
RED=`tput setaf 1`
GREEN=`tput setaf 2`
BLUE=`tput setaf 4`
RESET=`tput sgr0`

echo "${BLUE}== bc-crypto-base ==${RESET}"

pushd deps/bc-crypto-base
./configure --prefix ${SYSROOT}
make check
make install
popd

echo "${BLUE}== libwally-core ==${RESET}"

pushd deps/libwally-core
./tools/autogen.sh

if [[ "$OSTYPE" == "msys" ]]; then
# FIXME: figure out how to disable PTHREADS instead of tests
./configure --disable-tests --disable-shared --prefix ${SYSROOT}
else
./configure --disable-shared --prefix ${SYSROOT}
fi

make
make check
make install
popd

echo "${BLUE}== argp-standalone ==${RESET}"

pushd deps/argp-standalone/argp-standalone
set +e
patch -N <../patch-argp-fmtstream.h
set -e
./configure --prefix ${SYSROOT}
make install
cp libargp.a ${SYSROOT}/lib/
cp argp.h ${SYSROOT}/include/
popd

echo "${BLUE}== keytool ==${RESET}"

./configure
make clean
make check
echo "${GREEN}*** Keytool built.${RESET}"
echo "${GREEN}Next step: sudo make install${RESET}"
