@echo off
rem Setup device to compile

rem First command-line parameter (%1) of this batch contains device name (e.g. picopad10).
rem This batch is called from the _c1.bat file.

rem Use configuration name as parameter of the batch, to select target device:
rem   picopad20 ......... PicoPad version 2.0 (ARM) with TFT RGB565 output
rem   (empty) ........... default compilation

rem Move ":default" label before configuration, which you want to use as default.

if "%1"=="picopad20" goto picopad20
if "%1"=="" goto default

echo.
echo Incorrect configuration "%1"!
echo Press Ctrl+C to break compilation.
pause
goto default


:default
:picopad20
set DEVICE=picopad20
set DEVCLASS=picopad
set DEVDIR=!PicoPad20
exit /b


