@echo off

Setlocal EnableDelayedExpansion

if NOT EXIST build mkdir build
if NOT EXIST build\int mkdir build\int

if EXIST ../ctime %cd%/../ctime/ctime.exe -begin Tools/build_time.ctm

where /q cl.exe
if ERRORLEVEL 1 (
    echo Setting up MSBuildTools!
    rem call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    call "C:\BuildTools\devcmd.bat"
    echo ==========================================
)

set StopBuild=0

pushd Tools\shader_tool
call build.bat
popd
set LastError=!ERRORLEVEL!
if !ERRORLEVEL! NEQ 0 (
    echo Error:!ERRORLEVEL!
    set StopBuild=1
    echo Stopping build!
) else (
    echo Shader tool built succesfully! Generating Engine ShaderSrc
    Tools\shader_tool\bin\shader_tool.exe Game\run_tree\Data\Shaders\ Engine\src\Engine\Renderer\ShaderSrc\shaders_generated -quiet
)

rem if !StopBuild! NEQ 1 (
rem     pushd GameCode
rem     call build.bat
rem     popd
rem     set LastError=!ERRORLEVEL!
rem     if !ERRORLEVEL! NEQ 0 (
rem         echo Error:!ERRORLEVEL!
rem         set StopBuild=1
rem         echo Stopping build!
rem     )
rem )

if !StopBuild! NEQ 1 (
    pushd Engine
    call build.bat
    popd
    set LastError=!ERRORLEVEL!
    if !ERRORLEVEL! NEQ 0 (
        echo Error:!ERRORLEVEL!
        set StopBuild=1
        echo Stopping build!
    )
)

if !StopBuild! NEQ 1 (
    pushd RohinGame
    call build.bat
    popd
    set LastError=%ERRORLEVEL%
    if !ERRORLEVEL! NEQ 0 (
        echo Error:!ERRORLEVEL!
        set StopBuild=1
        echo Stopping build!
    )
)

if !StopBuild! NEQ 1 (
    echo ==========================================
    echo Everything built succesfully.
)

if EXIST ../ctime %cd%/../ctime/ctime.exe -end Tools/build_time.ctm  !LastError!
