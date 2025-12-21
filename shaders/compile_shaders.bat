@echo off
REM Shader compilation batch script for Windows using glslc (part of Vulkan SDK)
REM This script compiles GLSL shaders to SPIR-V format for use with SDL3 GPU

where glslc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: glslc not found. Please install the Vulkan SDK.
    echo Download from: https://vulkan.lunarg.com/sdk/home
    exit /b 1
)

echo Compiling shaders to SPIR-V...

REM Compile vertex shaders
for %%f in (*.vert) do (
    echo Compiling %%f -^> %%~nf.vert.spv
    glslc -fshader-stage=vertex "%%f" -o "%%~nf.vert.spv"
)

REM Compile fragment shaders
for %%f in (*.frag) do (
    echo Compiling %%f -^> %%~nf.frag.spv
    glslc -fshader-stage=fragment "%%f" -o "%%~nf.frag.spv"
)

echo Shader compilation complete!
pause
