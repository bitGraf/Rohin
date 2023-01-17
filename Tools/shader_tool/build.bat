@echo off
rem WUN WITH: Tools/shader_tool/unity_build/shader_tool.exe Game/run_tree/Data/Shaders/ Game/src/ShaderSrc/shaders_generated.cpp
rem echo Building!

if not defined DevEnvDir (
    echo Not set up!
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

echo ==========================================

IF NOT EXIST unity_build mkdir unity_build
pushd unity_build

rem Build shader_tool.exe
set ShaderToolCompileFlags=/MTd /nologo /Gm- /GR- /EHsc /Odi /std:c++14 /W4 /WX /wd4189 /wd4201 /wd4100 /FC
cl ..\shader_tool.cpp %ShaderToolCompileFlags% /link /SUBSYSTEM:CONSOLE /incremental:no /opt:ref user32.lib
echo hehe

popd


rem rem Compile shader tool to and run it to generate some code
rem set ShaderToolCompileFlags=/MTd /nologo /Gm- /GR- /EHsc /Odi /std:c++14 /W4 /WX /wd4189 /wd4201 /wd4100 /FC
rem cl ..\Tools\shader_tool\shader_tool.cpp %ShaderToolCompileFlags% /link /SUBSYSTEM:CONSOLE /incremental:no /opt:ref user32.lib
rem shader_tool.exe ../Game/run_tree/Data/Shaders/ ../Game/src/ShaderSrc/shaders_generated.cpp