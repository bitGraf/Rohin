@echo off

echo ***************************
echo *** G E N E R A T I N G ***
echo ***************************
echo.

call Tools\Premake\premake5.exe vs2019

rem for /f "delims=" %%D in ('dir /a:d /b "%programfiles(x86)%/Microsoft Visual Studio/"') do (
rem     echo %%~fD)

echo.
echo *************************************
echo *** D O N E   G E N E R A T I N G ***
echo *************************************