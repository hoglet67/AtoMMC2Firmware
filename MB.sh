#!/bin/bash

#SET BIN=C:\UTILS
export MCC=/opt/microchip/mplabc18/v3.40

rm -f *.cof
rm -f *.hex
rm -f *.lst
rm -rf  intermediate
mkdir intermediate
rm -rf picfirmware-bin
mkdir picfirmware-bin

#./genbuild.sh

BUILD="#define BUILDNUMBER movff 0"

# To build firmware 2D for the V2 hardware
echo "$BUILD,70" > buildnumber.h
./PICFirmware-build2.sh 25 5

# To build firmware 2E for the V3Pre hardware
#echo "$BUILD,71" > buildnumber.h
#./PICFirmware-build2.sh 25 5 -DATOMMC3MINUS

# To build firmware 2F for the V3/V4 hardware
#echo "$BUILD,72" > buildnumber.h
#./PICFirmware-build2.sh 25 5 -DATOMMC3PLUS

# rmdir -rf intermediate
