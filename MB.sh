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
./genbuild.sh
./PICFirmware-build2.sh 25 5

# rmdir -rf intermediate
