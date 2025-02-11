@echo off
cls

set COMPILER=cl
set FLAGS=/EHsc /MD /I"."

set SOURCE=BadSector.c
set OUTPUT=BadSector.exe

echo Compiling %SOURCE%...
%COMPILER% %FLAGS% %SOURCE% /Fe%OUTPUT%


if %errorlevel% equ 0 (
    echo Compilation successful! Output: %OUTPUT%
) else (
    echo Compilation failed. Check the errors above.
    exit /b %errorlevel%
)

echo Cleaning up intermediate files...
DEL BadSector.obj

