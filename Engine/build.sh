#! /usr/bin/bash

Name=rohin
WarningFlags='-Wall -Werror -Wno-unused' #/wd4189 /wd4201 /wd4100
Definitions='-D_DEBUG -DRH_EXPORT -D_CRT_SECURE_NO_WARNINGS -DROHIN_INTERNAL'
CompilerFlags='-c -fPIC -fno-rtti -fno-exceptions -O0 -std=c++14 -g '$WarningFlags' '$Definitions
IncludeDirs='-I../../Engine/src -I../../Game/src -I../../Engine/deps/math_lib/include -I../../Engine/deps/stb'
#LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% user32.lib Gdi32.lib Winmm.lib opengl32.lib
#ObjectFlags=/Fo../build/int/

pushd ../build/int
# Get a list of all the .c(pp) files.
SrcFiles=$(find ../../Engine/src \( -name "*.c" -o -name "*.cpp" \))

echo engine
echo '====================================='
echo 'name          =>' $Name
#echo 'warning-flags =>' $WarningFlags
#echo 'defs          =>' $Definitions
#echo 'compile-flags =>' $CompilerFlags
#echo 'include-dirs  =>' $IncludeDirs
#echo 'linker-flags  =>' $LinkerFlags
#echo 'object-flags  =>' $ObjectFlags
##echo 'src-files     =>' $SrcFiles
echo '====================================='

echo -n 'Building the Engine...'
if (g++ $CompilerFlags $IncludeDirs $ObjectFlags $SrcFiles); then
    echo "Success!"
else
    echo "Failed to compile..."
    popd
    return 0
fi

ObjFiles=$(find . \( -name "*.o" \))

echo -n Linking the Engine...
if g++ -shared -o ../lib$Name.so $ObjFiles; then
    echo "Success!"
    objdump -x ../lib$Name.so > ../$Name.map
else
    echo "Failed to link..."
    popd
    return 0
fi

popd
return 1