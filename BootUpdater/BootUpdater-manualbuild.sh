#!/bin/bash

#SET BIN=C:\UTILS
export MCC=/opt/microchip/mplabc18/v3.40

rm -f *.cof
rm -f *.hex
rm -f *.lst
rm -rf  intermediate
mkdir intermediate
rm -rf BootUpdater-bin
mkdir BootUpdater-bin
./BootUpdater-build2.sh 25
