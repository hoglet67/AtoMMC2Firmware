#!/bin/bash

# add -O- to disable optimization

${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "PicFirmware.c" -fo="intermediate/PicFirmware.o" --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "atmmc2core.c"  -fo="intermediate/mmc2_core.o"   --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "atmmc2wfn.c"   -fo="intermediate/mmc2_wfn.o"    --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "diskio.c"      -fo="intermediate/diskio.o"      --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "mmcio.c"       -fo="intermediate/mmcio.o"       --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "ff.c"          -fo="intermediate/ff.o"          --extended -k -O-
${MCC}/bin/mcc18 -p=18F45$1 $3 -I. -I${MCC}/h "wildcard.c"    -fo="intermediate/wildcard.o"    --extended -k -O-

${MCC}/bin/mplink -i -p18F45$1 -l. -l${MCC}/lib "intermediate/PicFirmware.o" "intermediate/ff.o" "intermediate/diskio.o" "intermediate/mmcio.o" "intermediate/mmc2_core.o" "intermediate/mmc2_wfn.o" "intermediate/wildcard.o" -u _CRUNTIME -u _EXTENDEDMODE -u __MPLAB_BUILD=1 -w -m "PICFirmware.map" -o"PICFirmware.cof"

java -jar java/pichextobin/pichextobin.jar PICFirmware.hex atommc$1.bin
