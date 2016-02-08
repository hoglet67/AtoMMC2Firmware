rmdir /s /q intermediate
mkdir intermediate
rmdir /s /q bootupdater-bin
mkdir bootupdater-bin
call bootupdater-build2.bat 20
call bootupdater-build2.bat 23
call bootupdater-build2.bat 25
rmdir /s /q intermediate
del *.cof
pause
