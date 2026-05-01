SET BIN=C:\UTILS
SET MCC=D:\MCC18

del *.cof
del *.hex
del *.lst
rmdir /s /q intermediate
mkdir intermediate
rmdir /s /q picfirmware-bin
mkdir picfirmware-bin
call genbuild

rem To build firmware 2D for the V2 hardware
call picfirmware-build2.bat 25 5

rem To build firmware 2E for the V3Pre hardware
rem  picfirmware-build2.bat 25 5 -DATOMMC3MINUS

rem To build firmware 2F for the V3/V4 hardware
rem call picfirmware-build2.bat 25 5 -DATOMMC3PLUS

rem rmdir /s /q intermediate
pause

