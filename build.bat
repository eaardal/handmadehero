@echo off

mkdir build
pushd build
cl ..\code\win32_handmade.cpp
popd
