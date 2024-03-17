@echo off

echo "Batch File:"
if EXIST ..\..\ctime ..\..\ctime\ctime.exe -stats build_time_bat.ctm

echo "Cmake:"
if EXIST ..\..\ctime ..\..\ctime\ctime.exe -stats build_time_cmake.ctm