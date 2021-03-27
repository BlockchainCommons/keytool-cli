#!/bin/bash
UR=${1}
# echo ${UR}
BYTEWORDS=`echo ${UR} | sed -e "s/^.*\///"`
# echo ${BYTEWORDS}
HEX=`seedtool --in btwm ${BYTEWORDS}`
# echo ${HEX}
DIAG=`cbor2diag -x ${HEX}`
echo ${DIAG}
