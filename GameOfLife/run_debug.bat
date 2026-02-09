@echo off
setlocal enabledelayedexpansion

set INPUT=samples\input.txt
set OUTPUT=samples\output.txt

echo ==========================================
echo Conway's Game of Life (Life 1.06)
echo Runs EXACTLY 10 iterations
echo ==========================================

set EXE=x64\Debug\GameOfLife.exe
if exist "%EXE%" goto :haveexe
set EXE=x64\Release\GameOfLife.exe
if exist "%EXE%" goto :haveexe

echo ERROR: executable not found.
echo Build solution first (Debug x64 or Release x64).
echo Expected:
echo   x64\Debug\GameOfLife.exe
echo or
echo   x64\Release\GameOfLife.exe
echo.
pause
exit /b 1

:haveexe
if not exist "%INPUT%" (
  echo ERROR: %INPUT% not found.
  echo Create it using the sample from the task statement.
  echo.
  pause
  exit /b 1
)

echo Using EXE  : %EXE%
echo Using INPUT: %INPUT%
echo OUTPUT     : %OUTPUT%
echo ------------------------------------------

echo INPUT preview (first 40 lines):
echo ------------------------------------------
powershell -NoProfile -Command "Get-Content -Path '%INPUT%' -TotalCount 40"
echo ------------------------------------------

echo Running simulation...
"%EXE%" < "%INPUT%" > "%OUTPUT%"

if errorlevel 1 (
  echo ERROR: program returned non-zero exit code.
  echo.
  pause
  exit /b 1
)

echo ------------------------------------------
echo OUTPUT preview (first 40 lines):
echo ------------------------------------------
powershell -NoProfile -Command "Get-Content -Path '%OUTPUT%' -TotalCount 40"
echo ------------------------------------------

for /f %%i in ('powershell -NoProfile -Command "(Get-Content -Path '%INPUT%' | Measure-Object -Line).Lines"') do set IN_LINES=%%i
for /f %%i in ('powershell -NoProfile -Command "(Get-Content -Path '%OUTPUT%' | Measure-Object -Line).Lines"') do set OUT_LINES=%%i

set /a IN_ALIVE=IN_LINES-1
set /a OUT_ALIVE=OUT_LINES-1

echo SUMMARY
echo   Alive cells (input) : %IN_ALIVE%
echo   Alive cells (output): %OUT_ALIVE%
echo ------------------------------------------
echo Full output saved to: %OUTPUT%
echo ------------------------------------------
pause
