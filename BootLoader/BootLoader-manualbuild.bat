rmdir /s /q intermediate
mkdir intermediate
del *.hex
del *.bin
call BootLoader-build2.bat 20
call BootLoader-build2.bat 23
call BootLoader-build2.bat 25
rmdir /s /q intermediate
del *.cof
pause
