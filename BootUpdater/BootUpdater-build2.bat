"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "BootUpdater.c" -fo="intermediate\BootUpdater.o" --extended -k -O-
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "pff.c" -fo="intermediate\pff.o" --extended -k -O-
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "device_io.c" -fo="intermediate\device_io.o" --extended -k -O-
"%MCC%\bin\mplink.exe" /p18F45%1 /l"." /l"%MCC%\lib" "intermediate\BootUpdater.o" "intermediate\pff.o" "intermediate\device_io.o" /u_CRUNTIME /u_EXTENDEDMODE /z__MPLAB_BUILD=1 /w  /o"BootUpdater.cof"
%BIN%\hexdump-d BootUpdater.hex BootUpdater-bin\atommc%1.bin
del BootUpdater.hex
