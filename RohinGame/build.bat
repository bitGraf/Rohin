@echo off
SetLocal EnableDelayedExpansion

rem set SrcFiles=src/rohin_game.cpp
REM Get a list of all the .cpp files.
pushd src
SET SrcFiles=
FOR /R %%f in (*.cpp) do (
    SET SrcFiles=!SrcFiles! %%f
)
popd

set Name=game
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set Definitions=/D_DEBUG /D_CRT_SECURE_NO_WARNINGS /DRH_INTERNAL
set CompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi /std:c++14 %WarningFlags% /FC /Z7 %Definitions%
set IncludeDirs=/Isrc /I../Engine/src /I../Game/src /I../Engine/deps/math_lib/include /I../Engine/deps/imgui
set LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% /LIBPATH:"../build" rohin.lib
set ObjectFlags=/Fo../build/int/

echo =====================================
echo Building the Windows entry point...

cl %IncludeDirs% %CompilerFlags% %SrcFiles% /Fm../build/%Name%.map /Fe: ../build/%Name%.exe %ObjectFlags% /link %LinkerFlags%

EXIT /B %ERRORLEVEL%