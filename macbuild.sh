#!/bin/bash

# OSX c18 compiler here:
# https://github.com/Manouchehri/Microchip-C18-Lite
# download, unpack, install to default location
# add path to compiler bin folder - Applications/microchip/mplabc18/v3.47/bin - to ~/.bashrc
# restart terminal or source ~/.bashrc

function compile {
  $MCC/bin/mcc18 -p$PART -I"./" -I"$MCC/h" --extended -pa=8 $1.c -fo=$1.o
  export OBJS="$OBJS $1.o"
}

OBJS=
MCC=/Applications/microchip/mplabc18/v3.47
PART=18F4525
NAME=atommc

buildnum=$(cat buildnumbermac.txt)
buildnum=${buildnum//[^0-9]/}
typeset -i ibn=$buildnum
ibn=$((ibn+1))
echo -n $ibn >buildnumbermac.txt
echo "#define BUILDNUMBER movff 0,$ibn" >buildnumber.h

compile PicFirmware
compile atmmc2core
compile atmmc2wfn
compile diskio
compile ff
compile mmcio
compile wildcard

$MCC/bin/mplink -p$PART -k"." -l"." -l"$MCC/lib" $OBJS -u_CRUNTIME -u_EXTENDEDMODE -z__MPLAB_BUILD=1 -i -m"$NAME.map" -o"$NAME.cof"

#hex2bin $NAME.hex out=zxpandfw.bin lo=0x1400

rm *.o
