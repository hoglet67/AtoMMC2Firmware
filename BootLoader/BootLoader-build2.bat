"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"%MCC%\h" "BootLoader.c" -fo=".\intermediate\BootLoader.o" --extended -k
"%MCC%\bin\mplink.exe" /p18F45%1 /l"%MCC%\lib" /k"." "intermediate\BootLoader.o" /u_CRUNTIME /u_EXTENDEDMODE /z__MPLAB_BUILD=1 /m"BootLoader.map" /w /o"BootLoader.cof"
rename BootLoader.hex BootLoader-%1.hex
