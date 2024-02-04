@echo off
set vc_build_path="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
call %vc_build_path%\vcvarsall.bat x64

if "%0" == "shell" (
    start "" "C:\Program Files\PureDevSoftware\10x\10x.exe"
)

if NOT "%0" == "shell" (
    echo "clicked on this!"
    cmd /k
)