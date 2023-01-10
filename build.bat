@echo off
rem RUN WITH: unity_build/win32_entry.exe -r W:\Rohin\Game\run_tree
rem echo Building!

if not defined DevEnvDir (
    echo Not set up!
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

echo ==========================================

rem /std:c++17
set CompileDefinitions=/DROHIN_INTERNAL=1 /DROHIN_SLOW=1 /DROHIN_WIN32=1
set WarningFlags=/W4 /WX /wd4189 /wd4201 /wd4100
set CommonCompilerFlags=/MTd /nologo /Gm- /GR- /EHa- /Odi /std:c++14 %WarningFlags% /FC /Z7 %CompileDefinitions%
set CommonLinkerFlags=/incremental:no /opt:ref

set EnginePath=Engine\src\Engine
set GamePath=Game\src\Game

IF NOT EXIST unity_build mkdir unity_build
pushd unity_build

rem Compile shader tool to and run it to generate some code
set ShaderToolCompileFlags=/MTd /nologo /Gm- /GR- /EHsc /Odi /std:c++14 /W4 /WX /wd4189 /wd4201 /wd4100 /FC
rem cl ..\Tools\shader_tool\shader_tool.cpp %ShaderToolCompileFlags% /link /SUBSYSTEM:CONSOLE /incremental:no /opt:ref user32.lib
rem shader_tool.exe ../Game/run_tree/Data/Shaders/ ../Game/src/ShaderSrc/shaders_generated.cpp

del *.pdb > NUL 2> NUL

rem Build the Game.dll
echo WAITING FOR PDB > lock.tmp
cl /I..\Engine\deps\stb /I..\Game\src /I..\Engine\src /I..\Engine\deps\math_lib\include %CommonCompilerFlags% ..\%GamePath%\game.cpp /Fmgame.map /LD /link %CommonLinkerFlags% /PDB:game_%random%.pdb /EXPORT:GetGameAPI
del lock.tmp

rem Build the Engine.exe
cl /I..\Engine\deps\stb /I..\Game\src /I..\Engine\src /I..\Engine\deps\math_lib\include /I..\Engine\src\Engine\Platform\GLAD\include %CommonCompilerFlags% ..\%EnginePath%\Platform\win32\win32_entry.cpp /Fmwin32_entry.map /link /SUBSYSTEM:CONSOLE %CommonLinkerFlags% user32.lib Gdi32.lib Winmm.lib opengl32.lib

popd
