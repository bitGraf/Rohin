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
set pdb_name=%Name%_%random%
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set Definitions=/D_DEBUG /DGAME_EXPORT /D_CRT_SECURE_NO_WARNINGS
set CompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi /std:c++14 %WarningFlags% /FC /Z7 %Definitions% /LD
set IncludeDirs=/Isrc /I../Engine/src /I../RohinApp/src /I../Engine/deps/math_lib/include /I../Engine/deps/imgui
set LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% /PDB:../bin/lib%pdb_name%.pdb /LIBPATH:"../bin" rohin.lib
set ObjectFlags=/Fo../bin/int/

echo =====================================
echo Building the Game code...

del ..\bin\lib%Name%_*.pdb > NUL 2> NUL

echo running > ..\bin\lock.tmp

rem cl %IncludeDirs% %CompilerFlags% %SrcFiles% /Fm../bin/%Name%.map /Fe: ../bin/lib%Name%.dll %ObjectFlags% /link %LinkerFlags%
cl %IncludeDirs% %CompilerFlags% %SrcFiles% /Fe: ../bin/lib%Name%.dll %ObjectFlags% /link %LinkerFlags%

del ..\bin\lock.tmp