REM File: compile_shaders.bat

@echo off
REM Shader compilation batch script for Windows using glslc (part of Vulkan SDK)
REM This script compiles GLSL shaders to SPIR-V format for use with SDL3 GPU

REM Change to the directory where this script is located
cd /d "%~dp0"

where glslc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: glslc not found. Please install the Vulkan SDK.
    echo Download from: https://vulkan.lunarg.com/sdk/home
    exit /b 1
)

echo Compiling shaders to SPIR-V...
echo Working directory: %CD%
echo Shader directory: Resources\shaders
echo Output directory: Resources\shaders\compiled

REM Change to shaders directory
cd Resources\shaders

REM Create compiled directory if it doesn't exist
if not exist "compiled" mkdir compiled

REM Compile vertex shaders
for %%f in (*.vert) do (
    echo Compiling %%f -^> compiled\%%~nf.vert.spv
    glslc -fshader-stage=vertex "%%f" -o "compiled\%%~nf.vert.spv"
    if errorlevel 1 (
        echo   ERROR: Failed to compile %%f
    )
)

REM Compile fragment shaders
for %%f in (*.frag) do (
    echo Compiling %%f -^> compiled\%%~nf.frag.spv
    glslc -fshader-stage=fragment "%%f" -o "compiled\%%~nf.frag.spv"
    if errorlevel 1 (
        echo   ERROR: Failed to compile %%f
    )
)

echo.
echo Checking for compiled shaders in: %CD%\compiled
dir compiled\*.spv 2>nul
if errorlevel 1 (
    echo No .spv files found in compiled directory.
)

echo Shader compilation complete!
pause
