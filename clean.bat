@echo off

rmdir /S /Q rpr\obj\Release
rmdir /S /Q rpr\obj\Debug
rmdir /S /Q rpr\build
del /Q rpr\lib\*rpr*
rmdir /S /Q rpr\project\.vs
del /Q rpr\project\*.user

for /D %%I IN (tests tests_with_crt) DO call :clean_project %%I

goto :eof

:clean_project

rmdir /S /Q samples\%1\obj\Release
rmdir /S /Q samples\%1\obj\Debug
rmdir /S /Q samples\%1\build
del /Q samples\%1\bin\*%1*
rmdir /S /Q samples\%1\project\*.vs
del /Q samples\%1\project\*.user
