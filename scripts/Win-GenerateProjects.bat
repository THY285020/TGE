@echo off
pushd ..\
call Vendor\bin\premake\premake5.exe vs2019
popd
PAUSE 