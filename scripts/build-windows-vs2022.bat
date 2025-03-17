@echo off

pushd ..
call vendor\bin\premake\Windows\premake5.exe vs2022
popd

PAUSE