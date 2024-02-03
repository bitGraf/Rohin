@echo off

Setlocal EnableDelayedExpansion

if NOT EXIST build mkdir build
if NOT EXIST build\int mkdir build\int

if EXIST ../ctime %cd%/../ctime/ctime.exe -begin Tools/build_time.ctm

set StopBuild=0

rem build the dynamic game-code dll
rem not doing this feature yet!
rem pushd GameCode
rem call build.bat
rem popd
rem set LastError=%ERRORLEVEL%
rem IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && set StopBuild=1)

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
