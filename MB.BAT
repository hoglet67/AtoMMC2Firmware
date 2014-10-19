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
call picfirmware-build2.bat 25 5
rem rmdir /s /q intermediate
pause
