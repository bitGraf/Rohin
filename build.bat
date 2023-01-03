@echo off
rem echo Building!

if not defined DevEnvDir (
    echo Not set up!
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

echo ==========================================

set CompileDefinitions=/DHANDMADE_INTERNAL=1 /DHANDMADE_SLOW=1 /DHANDMADE_WIN32=1
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set CommonCompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi %WarningFlags% /FC /Z7 %CompileDefinitions%
set CommonLinkerFlags=/incremental:no /opt:ref

set EnginePath=Engine\src\Engine
set GamePath=Engine\src\Game

IF NOT EXIST unity_build mkdir unity_build
pushd unity_build

del *.pdb > NUL 2> NUL

rem rem 64-bit Build

rem engine path:
rem echo ..\%EnginePath%\Platform\win32\win32_entry.cpp
rem game path:
rem echo ..\%GamePath%\game.cpp

rem Build the Game.dll
echo WAITING FOR PDB > lock.tmp
cl /IW:\Rohin\Engine\src %CommonCompilerFlags% ..\%GamePath%\game.cpp /LD /link %CommonLinkerFlags% /PDB:game_%random%.pdb /EXPORT:GetGameAPI
del lock.tmp

rem Build the Engine.exe
cl /IW:\Rohin\Engine\src %CommonCompilerFlags% ..\%EnginePath%\Platform\win32\win32_entry.cpp /link %CommonLinkerFlags% user32.lib Gdi32.lib Winmm.lib opengl32.lib

popd
