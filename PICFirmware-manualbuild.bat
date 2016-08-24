rmdir /s /q intermediate
mkdir intermediate
rmdir /s /q picfirmware-bin
mkdir picfirmware-bin
call genbuild
call picfirmware-build2.bat 25 5
rmdir /s /q intermediate
del *.cof
del *.hex
del *.lst
pause
