@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=2 delims=:" %%A in (`curl -s "https://api.github.com/repos/TLR2025/Leetcode-I-O-Helper/releases/latest" ^| findstr /i "tag_name"`) do set "version_raw=%%A"

if not defined version_raw (
  echo ERROR: Unable to obtain version string from GitHub API. Please check if the network is correct.
  exit /b 1
)
set "version=%version_raw: =%"
set "version=%version:"=%"
set "version=%version:,=%"

echo Latest version: %version%

set "tmpfile=%TEMP%\lioh.exe"
set "target=%USERPROFILE%\lioh.exe"
set "download_url=https://github.com/TLR2025/Leetcode-I-O-Helper/releases/download/%version%/lioh-windows-msvc-%version%.exe"

echo Downloading %download_url% ...
curl -fsSL "%download_url%" -o "%tmpfile%"
if errorlevel 1 (
  echo ERROR: Download failed!
  del "%tmpfile%" 2>nul
  exit /b 1
)

move /y "%tmpfile%" "%target%" >nul
if errorlevel 1 (
  echo ERROR: Unable to move file to %target%. Please check if you have write permissions.
  exit /b 1
)

echo Downloaded to %target%.

pause

endlocal
