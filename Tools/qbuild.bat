@echo off
cls

echo ***********************
echo *** B U I L D I N G ***
echo ***********************
echo.

rem BUILD HERE
pushd ..\build
cmake --build . --config Release --target ALL_BUILD
popd
rem END BUILDING

echo.
echo *********************************
echo *** D O N E   B U I L D I N G ***
echo *********************************