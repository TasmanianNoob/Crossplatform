#!/bin/bash

# Shader Profile
SHADER_PROFILE="vulkan"
if [ ! -z "$1" ]; then
  SHADER_PROFILE=$1
fi

shaderc() {
    PATH_TO_SHADERS="../"

    if [ -e "${PATH_TO_SHADERS}/${1}.bin" ]; then
        return 0
    fi

    ${PATH_TO_SHADERS}/shaderc -f ${PATH_TO_SHADERS}/${1}.sh -o ${PATH_TO_SHADERS}/${1}.bin --platform osx --profile ${SHADER_PROFILE} --type ${2} -i ${PATH_TO_SHADERS} --verbose

    if [ $? -ne 0 ]; then
        exit $?
    fi

    return 0
}

# Vertex Shaders
shaderc vs_main vertex

# Fragment Shaders
shaderc fs_colour_rmlui fragment

exit 0