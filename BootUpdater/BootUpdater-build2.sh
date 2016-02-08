#!/bin/bash

${MCC}/bin/mcc18 -p=18F45$1 -DLINUX_BUILD=1 -I. -I${MCC}/h "BootUpdater.c" -fo="intermediate/BootUpdater.o" --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 -DLINUX_BUILD=1 -I. -I${MCC}/h "pff.c" -fo="intermediate/pff.o" --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 -DLINUX_BUILD=1 -I. -I${MCC}/h "device_io.c" -fo="intermediate/device_io.o" --extended -k -O-
${MCC}/bin/mplink -i -p18F45$1 -l. -l${MCC}/lib "intermediate/BootUpdater.o" "intermediate/pff.o" "intermediate/device_io.o" -u _CRUNTIME -u _EXTENDEDMODE -u __MPLAB_BUILD=1 -w -o"BootUpdater.cof"

java -jar ../java/pichextobin/pichextobin.jar BootUpdater.hex BootUpdater-bin/atommc$1.bin
