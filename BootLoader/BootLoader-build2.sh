#!/bin/bash

# add -O- to disable optimization

${MCC}/bin/mcc18 -p=18F45$1 -DLINUX_BUILD=1 -I. -I${MCC}/h "BootLoader.c" -fo="intermediate/BootLoader.o" --extended -k
${MCC}/bin/mplink -i -p18F45$1 -l. -l${MCC}/lib "intermediate/BootLoader.o" -u _CRUNTIME -u _EXTENDEDMODE -u __MPLAB_BUILD=1 -w -m "BootLoader.map" -o"BootLoader.cof"

java -jar ../java/pichextobin/pichextobin.jar BootLoader.hex bootloader-bin/bootld$1.bin bootloader

