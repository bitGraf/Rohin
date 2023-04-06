#! /usr/bin/bash

#if EXIST ../ctime W:\ctime\ctime.exe -begin Tools/build_time.ctm

StopBuild=0

# build the dynamic game-code dll
# not doing this feature yet!
# pushd GameCode
# call build.bat
# popd
# set LastError=%ERRORLEVEL%
# IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && set StopBuild=1)

echo top level build
if [ $StopBuild -ne 1 ]
then
    pushd Engine
    source build.sh
    LastError=$?
    popd
    if [ $LastError -ne 1 ]; then 
        StopBuild=1
        echo 'Stopping build!'
    fi
fi

if [ $StopBuild -ne 1 ]; then
    pushd RohinGame
    source build.sh
    LastError=$?
    popd
    if [ $LastError -ne 1 ]; then 
        StopBuild=1
        echo 'Stopping build!'
    fi
fi

if [ $StopBuild -ne 1 ]; then
    echo ==========================================
    echo Everything built succesfully.
fi

#if EXIST ../ctime W:\ctime\ctime.exe -end Tools/build_time.ctm !LastError!
