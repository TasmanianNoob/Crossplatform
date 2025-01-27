@echo off

:: Platform
set PLATFORM=%1

:: Vertex Shaders
CALL :shaderc vs_main vertex

:: Fragment Shaders
CALL :shaderc fs_main fragment

EXIT /B 0

:shaderc
@REM     set PATH_TO_SHADERS=%CD%/../assets/

@REM     if exist %PATH_TO_SHADERS%/%1.bin EXIT /B 0
    mkdir dx10
    mkdir dx11
    mkdir glsl
    mkdir essl
    mkdir spirv
    shaderc.exe -f %1.sh -o dx10/%1.bin --platform %PLATFORM% --profile s_4_0 --type %2 -i ./ --verbose
    shaderc.exe -f %1.sh -o dx11/%1.bin --platform %PLATFORM% --profile s_5_0 --type %2 -i ./ --verbose
    shaderc.exe -f %1.sh -o glsl/%1.bin --platform %PLATFORM% --profile 440 --type %2 -i ./ --verbose
    shaderc.exe -f %1.sh -o essl/%1.bin --platform %PLATFORM% --profile 320_es --type %2 -i ./ --verbose
    shaderc.exe -f %1.sh -o spirv/%1.bin --platform %PLATFORM% --profile spirv --type %2 -i ./ --verbose
    if %errorlevel% neq 0 EXIT /B %errorlevel%
    EXIT /B 0