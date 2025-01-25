@echo off

:: Shader Profile
set SHADER_PROFILE=s_5_0
if not "%1"=="" set SHADER_PROFILE=%1

:: Vertex Shaders
CALL :shaderc vs_main vertex

:: Fragment Shaders
CALL :shaderc fs_main fragment

EXIT /B 0

:shaderc
    set PATH_TO_SHADERS=%CD%/../

@REM     if exist %PATH_TO_SHADERS%/%1.bin EXIT /B 0
    %PATH_TO_SHADERS%/shaderc.exe -f %PATH_TO_SHADERS%/%1.sh -o %PATH_TO_SHADERS%/%1.bin --platform windows --profile %SHADER_PROFILE% --type %2 -i %PATH_TO_SHADERS% --verbose
    if %errorlevel% neq 0 EXIT /B %errorlevel%
    EXIT /B 0