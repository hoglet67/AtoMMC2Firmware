@echo off
for /f "tokens=1" %%i in (buildnumber.txt) do set vsn=%%i
set /A VSN=VSN+1
echo %VSN% > buildnumber.txt
echo #define BUILDNUMBER movff 0,%VSN% > buildnumber.h
