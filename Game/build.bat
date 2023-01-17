@echo off
SetLocal EnableDelayedExpansion

REM Get a list of all the .cpp files.
pushd src
SET SrcFiles=
FOR /R %%f in (*.cpp) do (
    SET SrcFiles=!SrcFiles! %%f
)
popd


set Name=game
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set Definitions=/D_DEBUG /DGAME_EXPORT /D_CRT_SECURE_NO_WARNINGS
set CompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi /std:c++14 %WarningFlags% /FC /Z7 %Definitions% /LD
set IncludeDirs=/Isrc /I../Engine/src
set LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% /LIBPATH:"../build" rohin.lib
set ObjectFlags=/Fo../build/int/

echo =====================================
echo Building the Game code...

cl %IncludeDirs% %CompilerFlags% %SrcFiles% /Fm../build/%Name%.map /Fe: ../build/%Name%.dll %ObjectFlags% /link %LinkerFlags%