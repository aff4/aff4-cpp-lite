@echo off
setlocal

rem Directory Structure
mkdir Package
mkdir Package\include
mkdir Package\examples
mkdir Package\Win32
mkdir Package\Win32\Debug
mkdir Package\Win32\Release
mkdir Package\x64
mkdir Package\x64\Debug
mkdir Package\x64\Release

copy Package.txt Package\README.txt

rem Includes
copy ..\src\*.h Package\include

rem Examples
copy ..\examples\*.h Package\examples
copy ..\examples\*.cc Package\examples

rem Win32 Debug Libraries
copy Win32\Debug\libaff4.lib Package\Win32\Debug
copy Win32\Debug\snappy.lib Package\Win32\Debug
copy Win32\Debug\zlib1.lib Package\Win32\Debug
copy Win32\Debug\raptor2.lib Package\Win32\Debug
copy Win32\Debug\libaff4.dll Package\Win32\Debug
copy Win32\Debug\snappy.dll Package\Win32\Debug
copy Win32\Debug\zlib1.dll Package\Win32\Debug
copy Win32\Debug\raptor2.dll Package\Win32\Debug

rem Win32 Release Libraries
copy Win32\Release\libaff4.lib Package\Win32\Release
copy Win32\Release\snappy.lib Package\Win32\Release
copy Win32\Release\zlib1.lib Package\Win32\Release
copy Win32\Release\raptor2.lib Package\Win32\Release
copy Win32\Release\libaff4.dll Package\Win32\Release
copy Win32\Release\snappy.dll Package\Win32\Release
copy Win32\Release\zlib1.dll Package\Win32\Release
copy Win32\Release\raptor2.dll Package\Win32\Release

rem x64 Debug Libraries
copy x64\Debug\libaff4.lib Package\x64\Debug
copy x64\Debug\snappy.lib Package\x64\Debug
copy x64\Debug\zlib1.lib Package\x64\Debug
copy x64\Debug\raptor2.lib Package\x64\Debug
copy x64\Debug\libaff4.dll Package\x64\Debug
copy x64\Debug\snappy.dll Package\x64\Debug
copy x64\Debug\zlib1.dll Package\x64\Debug
copy x64\Debug\raptor2.dll Package\x64\Debug

rem x64 Release Libraries
copy x64\Release\libaff4.lib Package\x64\Release
copy x64\Release\snappy.lib Package\x64\Release
copy x64\Release\zlib1.lib Package\x64\Release
copy x64\Release\raptor2.lib Package\x64\Release
copy x64\Release\libaff4.dll Package\x64\Release
copy x64\Release\snappy.dll Package\x64\Release
copy x64\Release\zlib1.dll Package\x64\Release
copy x64\Release\raptor2.dll Package\x64\Release