@echo off 
@set CL=/D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE
@set LINK=/LARGEADDRESSAWARE

set VTKDir=C:\VTK_build\bin\Debug

PATH=%VTKDir%;%PATH%

FOR %%f IN (*.sln) DO %%f

