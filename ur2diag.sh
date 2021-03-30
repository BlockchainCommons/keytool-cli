#!/bin/bash

# Requires:
#   https://www.npmjs.com/package/cbor-cli
#   https://github.com/blockchaincommons/bc-seedtool-cli

UR=${1}
BYTEWORDS=`echo ${UR} | sed -e "s/^.*\///"`
HEX=`seedtool --in btwm ${BYTEWORDS}`
DIAG=`cbor2diag -x ${HEX}`
echo ${DIAG}
