@echo off
setlocal

rem Directory Structure
mkdir PackageXWays
mkdir PackageXWays\x64

copy xways.txt PackageXWays\README.txt

rem Win32 Release Libraries
copy Win32\Release\imageAFF4.dll PackageXWays\ImageIOAFF4.dll
copy Win32\Release\libaff4.dll PackageXWays\
copy Win32\Release\snappy.dll PackageXWays\
rem copy Win32\Release\zlib1.dll PackageXWays\
copy Win32\Release\raptor2.dll PackageXWays\

rem x64 Release Libraries
copy x64\Release\imageAFF4.dll PackageXWays\x64\ImageIOAFF4.dll
copy x64\Release\libaff4.dll PackageXWays\x64
copy x64\Release\snappy.dll PackageXWays\x64
rem copy x64\Release\zlib1.dll PackageXWays\x64
copy x64\Release\raptor2.dll PackageXWays\x64