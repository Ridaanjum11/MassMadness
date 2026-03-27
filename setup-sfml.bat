@echo off
REM This script guides you through SFML setup
echo =============================================
echo   SFML-3.0.2 Download Guide
echo =============================================
echo.
echo I will download SFML for you. This may take a moment...
echo.

REM Try multiple download sources
set SFML_URL=https://github.com/SFML/SFML/releases/download/3.0.2/SFML-3.0.2-windows-vc17-64-bit.zip
set DOWNLOAD_PATH=%USERPROFILE%\Downloads\SFML.zip
set SFML_DIR=C:\SFML-3.0.2

echo [1] Downloading from GitHub...
powershell -NoProfile -Command "try { [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; (New-Object Net.WebClient).DownloadFile('%SFML_URL%', '%DOWNLOAD_PATH%'); Write-Host 'Download successful!' } catch { Write-Host 'Download failed. Please download manually from: %SFML_URL%'; exit 1 }"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Automatic download failed.
    echo.
    echo Please download SFML manually:
    echo   1. Open: https://github.com/SFML/SFML/releases/tag/3.0.2
    echo   2. Download: SFML-3.0.2-windows-vc17-64-bit.zip
    echo   3. Extract to: C:\SFML-3.0.2
    echo   4. Run build.bat again
    echo.
    pause
    exit /b 1
)

echo [2] Extracting SFML...
if exist "%SFML_DIR%" (
    echo SFML already exists at %SFML_DIR%, skipping extraction
) else (
    powershell -NoProfile -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; $sfmlZip = '%DOWNLOAD_PATH%'; $extracted = 'C:\SFML-temp'; [System.IO.Compression.ZipFile]::ExtractToDirectory($sfmlZip, $extracted, $true); Get-ChildItem $extracted -Filter '*SFML*' -Directory | ForEach-Object { Move-Item $_.FullName '%SFML_DIR%' -Force }; if (!(Test-Path '%SFML_DIR%')) { Move-Item \"$extracted\*\" '%SFML_DIR%' -Force }; Remove-Item $extracted -Force -Recurse -ErrorAction SilentlyContinue"
    
    if %ERRORLEVEL% EQU 0 (
        echo Extraction successful!
    ) else (
        echo Extraction failed
        exit /b 1
    )
)

echo [3] Cleaning up...
del /Q "%DOWNLOAD_PATH%" >nul 2>&1

echo.
echo =============================================
echo ✓ SFML installed to %SFML_DIR%
echo =============================================
echo.
echo Run build.bat now to compile the project!
echo.
pause
