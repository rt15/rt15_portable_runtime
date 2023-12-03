@echo off

del /Q project\*.layout

rmdir /S /Q rpr\obj\Release
rmdir /S /Q rpr\obj\Debug
del /S /Q rpr\lib\*.a
del /S /Q rpr\lib\*.lib
del /S /Q rpr\lib\*.pdb
rmdir /S /Q rpr\project\.vs
del /Q rpr\project\*.layout
del /Q rpr\project\*.depend
del /Q rpr\project\*.user

for /D %%I IN (tests tests_with_crt) DO call :clean_project %%I

goto :eof

:clean_project

rmdir /S /Q samples\%1\obj\Release
rmdir /S /Q samples\%1\obj\Debug
del /Q samples\%1\bin\%1*
del /Q samples\%1\project\*.layout
del /Q samples\%1\project\*.depend

