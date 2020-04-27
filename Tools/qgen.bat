@echo off

echo ***************************
echo *** G E N E R A T I N G ***
echo ***************************
echo.

mkdir ..\build
mkdir ..\bin

rem BUILD HERE
pushd ..\build
cmake ..
popd
rem END BUILDING

echo.
echo *************************************
echo *** D O N E   G E N E R A T I N G ***
echo *************************************

PAUSE