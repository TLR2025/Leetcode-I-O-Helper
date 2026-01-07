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
set "target=%USERPROFILE%\.lioh\lioh.exe"
for %%i in ("%target%") do set "target_dir=%%~dpi"

set "download_url=https://github.com/TLR2025/Leetcode-I-O-Helper/releases/download/%version%/lioh-windows-msvc-%version%.exe"

echo Downloading %download_url% ...
curl -fsSL "%download_url%" -o "%tmpfile%"
if errorlevel 1 (
  echo ERROR: Download failed!
  del "%tmpfile%" 2>nul
  exit /b 1
)

mkdir "%target_dir%"
move /y "%tmpfile%" "%target%" >nul
if errorlevel 1 (
  echo ERROR: Unable to move file to %target%. Please check if you have write permissions.
  exit /b 1
)

echo Downloaded to %target%.

set PATH=%target_folder%;%PATH%
powershell -NoProfile -Command "$tf=[Environment]::ExpandEnvironmentVariables('%target_dir%');if(-not (Test-Path $tf)){New-Item -ItemType Directory -Path $tf|Out-Null};$u=[Environment]::GetEnvironmentVariable('Path','User');if(-not $u){[Environment]::SetEnvironmentVariable('Path',$tf,'User');Write-Host 'User PATH set to:' $tf} elseif($u.Split(';') -notcontains $tf){[Environment]::SetEnvironmentVariable('Path',$u + ';' + $tf,'User');Write-Host 'Appended to User PATH:' $tf} else {Write-Host 'Already in User PATH:' $tf}"

pause

endlocal
