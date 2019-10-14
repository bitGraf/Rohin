@echo off

echo ***********************
echo *** R U N N I N G ***
echo *********************
echo.

rem BUILD HERE
pushd ..\bin\Release
bigDisc.exe ..\..\run_tree
popd
rem END BUILDING

echo.
echo *******************************
echo *** D O N E   R U N N I N G ***
echo *******************************