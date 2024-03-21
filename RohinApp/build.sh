#! /usr/bin/bash

Name=game
WarningFlags='-Wall -Werror -Wno-unused' #/wd4189 /wd4201 /wd4100
Definitions='-D_DEBUG -DRH_INTERNAL'
CompilerFlags='-c -fPIC -fno-rtti -fno-exceptions -O0 -std=c++14 -g '$WarningFlags' '$Definitions
IncludeDirs='-I../../Engine/src -I../../RohinGame/src -I../../Engine/deps/math_lib/include'
#LinkerFlags='-L../ -l:librohin.so -Wl,-rpath,.'
LinkerFlags='-L../ -l:librohin.so'
#ObjectFlags=/Fo../build/int/

build_dir='../build/'$Name'-int'
if [ ! -d $build_dir ]; then
    mkdir -p $build_dir
fi
pushd $build_dir

# Get a list of all the .c(pp) files.
SrcFiles=$(find ../../RohinGame/src \( -name "*.c" -o -name "*.cpp" \))

#echo '====================================='
#echo 'name          =>' $Name
#echo 'warning-flags =>' $WarningFlags
#echo 'defs          =>' $Definitions
#echo 'compile-flags =>' $CompilerFlags
#echo 'include-dirs  =>' $IncludeDirs
#echo 'linker-flags  =>' $LinkerFlags
#echo 'object-flags  =>' $ObjectFlags
##echo 'src-files     =>' $SrcFiles
echo '====================================='

echo 'Building the Linux entry point!'
echo -n 'compiling...'
failed=0
if (g++ $CompilerFlags $IncludeDirs $ObjectFlags $SrcFiles); then
    echo "Success!"
else
    echo "Failed to compile!"
    popd
    failed=1
    return 0
fi

if [ $failed -eq 0 ]; then
    ObjFiles=$(find . \( -name "*.o" \))

    echo -n 'Linking...'
    if g++ -o ../$Name $LinkerFlags $ObjFiles; then
        echo "Success!"
        #objdump -x ../lib$Name.so > ../$Name.map
    else
        echo "Failed to link!"
        popd
        failed=1
        return 0
    fi
fi

if [ $failed -eq 0 ]; then
    popd
    return 1
fi