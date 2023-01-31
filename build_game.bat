@echo off

Setlocal EnableDelayedExpansion

if EXIST ../ctime W:\ctime\ctime.exe -begin Tools/build_time.ctm

set StopBuild=0

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

if EXIST ../ctime W:\ctime\ctime.exe -end Tools/build_time.ctm !LastError!
