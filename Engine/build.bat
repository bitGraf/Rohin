@echo off
SetLocal EnableDelayedExpansion

REM Get a list of all the .cpp files.
pushd src
SET SrcFiles=
FOR /R %%f in (*.cpp *.c) do (
    SET SrcFiles=!SrcFiles! %%f
)
popd


set Name=rohin
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set Definitions=/D_DEBUG /DRH_EXPORT /D_CRT_SECURE_NO_WARNINGS /DROHIN_INTERNAL /DROHIN_WINDOWS /DRH_COMPILE_OPENGL_4_4
set CompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi /std:c++14 %WarningFlags% /FC /Z7 %Definitions% /LD
set IncludeDirs=/Isrc /I../Game/src /Ideps/math_lib/include /Ideps/stb
set LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% user32.lib Gdi32.lib Winmm.lib opengl32.lib
set ObjectFlags=/Fo../build/int/

echo =====================================
echo Building the Engine...

cl %IncludeDirs% %CompilerFlags% %SrcFiles% /Fm../build/%Name%.map /Fe: ../build/%Name%.dll %ObjectFlags% /link %LinkerFlags%

EXIT /B %ERRORLEVEL%