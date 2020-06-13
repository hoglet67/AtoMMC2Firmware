set MCC=%MCC_INCLUDE%\..\

if "%MCC%"=="\..\" (
 echo Environment variable MCC is not set.
 echo SET MCC=[path to microchip C18 installation]
 exit /b
)

if not exist BIN\hexdump-d.exe (
 echo Can't find hex->binary converter.
 exit /b
)

"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "picFirmware.c" -fo="intermediate\picFirmware.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "atmmc2core.c" -fo="intermediate\mmc2_core.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "atmmc2wfn.c" -fo="intermediate\mmc2_wfn.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "diskio.c" -fo="intermediate\diskio.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "mmcio.c" -fo="intermediate\mmcio.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "ff.c" -fo="intermediate\ff.o" --extended -k %DISOPT%
"%MCC%\bin\mcc18.exe" -p=18F45%1 /i"." -I"%MCC%\h" "wildcard.c" -fo="intermediate\wildcard.o" --extended -k %DISOPT%

"%MCC%\bin\mplink.exe" /i /p18F45%1 /l"." /l"%MCC%\lib" "intermediate\picFirmware.o" "intermediate\ff.o" "intermediate\diskio.o" "intermediate\mmcio.o" "intermediate\mmc2_core.o" "intermediate\mmc2_wfn.o" "intermediate\wildcard.o" /u_CRUNTIME /u_EXTENDEDMODE /z__MPLAB_BUILD=1 /w /o"PICFirmware.cof"
BIN\HexDump-D.exe PICFirmware.hex picFirmware-bin\atommc%1.bin %2
rem del PICFirmware.hex
