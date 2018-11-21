@echo off
setlocal

:BUILD
msbuild /verbosity:minimal /t:rebuild /p:configuration=Debug /p:platform=x86
if errorlevel 1 goto errorDone
msbuild /verbosity:minimal /t:rebuild /p:configuration=Debug /p:platform=x64
if errorlevel 1 goto errorDone
msbuild /verbosity:minimal /t:rebuild /p:configuration=Release /p:platform=x86
if errorlevel 1 goto errorDone
msbuild /verbosity:minimal /t:rebuild /p:configuration=Release /p:platform=x64
if errorlevel 1 goto errorDone

EXIT /b

:errorDone
@echo "BUILD ERROR!"
@echo "BUILD ERROR!"
@echo "BUILD ERROR!"

:EOF
