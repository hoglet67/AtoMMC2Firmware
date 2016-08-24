@echo off
for /f "tokens=1" %%i in (buildnumberwin.txt) do set vsn=%%i
set /A VSN=VSN+1
echo %VSN% > buildnumberwin.txt
echo #define BUILDNUMBER movff 0,%VSN% > buildnumber.h
