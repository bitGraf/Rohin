@echo off

echo ***********************
echo *** B U I L D I N G ***
echo ***********************
echo.

rem BUILD HERE
pushd ..\Build
cmake --build . --config Release --target big-disc
popd
rem END BUILDING

echo.
echo *********************************
echo *** D O N E   B U I L D I N G ***
echo *********************************