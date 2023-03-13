Name=game
WarningFlags='-Wall -Werror -Wno-unused' #/wd4189 /wd4201 /wd4100
Definitions='-D_DEBUG -D_CRT_SECURE_NO_WARNINGS -DROHIN_INTERNAL'
CompilerFlags='-c -fPIC -fno-rtti -fno-exceptions -O0 -std=c++14 -g '$WarningFlags' '$Definitions
IncludeDirs='-I../../Engine/src -I../../RohinGame/src -I../../Engine/deps/math_lib/include'
#LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE %CommonLinkerFlags% user32.lib Gdi32.lib Winmm.lib opengl32.lib
#ObjectFlags=/Fo../build/int/

pushd ../build/int
# Get a list of all the .c(pp) files.
SrcFiles=$(find ../../RohinGame/src \( -name "*.c" -o -name "*.cpp" \))

#echo =====================================
#echo 'name          =>' $Name
#echo 'warning-flags =>' $WarningFlags
#echo 'defs          =>' $Definitions
#echo 'compile-flags =>' $CompilerFlags
#echo 'include-dirs  =>' $IncludeDirs
#echo 'linker-flags  =>' $LinkerFlags
#echo 'object-flags  =>' $ObjectFlags
##echo 'src-files     =>' $SrcFiles
echo =====================================

echo -n Building the Linux entry point...
if g++ $CompilerFlags $IncludeDirs $ObjectFlags $SrcFiles; then
    echo "Success!"
else
    echo "Failed to compile..."
    popd
    return 0
fi

ObjFiles=$(find . \( -name "*.o" \))

echo -n Linking...
if g++ -o ../$Name $ObjFiles; then
    echo "Success!"
    #objdump -x ../lib$Name.so > ../$Name.map
else
    echo "Failed to link..."
    popd
    return 0
fi

popd
return 1