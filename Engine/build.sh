#! /usr/bin/bash

Name=rohin
WarningFlags='-Wall -Werror -Wno-unused' #/wd4189 /wd4201 /wd4100
Definitions='-D_DEBUG -DROHIN_INTERNAL -DRH_EXPORT'
CompilerFlags='-c -fPIC -fno-rtti -fno-exceptions -O0 -std=c++14 -g '$WarningFlags' '$Definitions
IncludeDirs='-I../../Engine/src -I../../Engine/deps/math_lib/include -I../../Engine/deps/stb'
LinkerFlags='-lGL -lX11 -lX11-xcb' #user32.lib Gdi32.lib Winmm.lib opengl32.lib
#ObjectFlags=/Fo../build/int/

build_dir='../build/'$Name'-int'
if [ ! -d $build_dir ]; then
    mkdir -p $build_dir
fi
pushd $build_dir

# Get a list of all the .c(pp) files.
SrcFiles=$(find ../../Engine/src \( -name "*.c" -o -name "*.cpp" \))

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

echo 'Building the shared Engine library!'
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
    if g++ -shared -o ../lib$Name.so $ObjFiles $LinkerFlags; then
        echo "Success!"
        objdump -x ../lib$Name.so > ../$Name.map

        echo 'installing library to ~/lib'
        cp ../lib$Name.so ~/lib/
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