#!/bin/bash

# Shader Profile

PLATFORM=$1

shadercompile() {
#    PATH_TO_SHADERS="../assets/"

#    if [ -e "${1}.bin" ]; then
#        return 0
#    fi
    ./shaderc -f ${1}.sh -o glsl_${1}.bin --platform ${PLATFORM} --profile 440 --type ${2} -i ./ --verbose
    ./shaderc -f ${1}.sh -o essl_${1}.bin --platform ${PLATFORM} --profile 320_es --type ${2} -i ./ --verbose
    ./shaderc -f ${1}.sh -o msl_${1}.bin --platform ${PLATFORM} --profile metal --type ${2} -i ./ --verbose
    ./shaderc -f ${1}.sh -o spirv_${1}.bin --platform ${PLATFORM} --profile spirv --type ${2} -i ./ --verbose

    if [ $? -ne 0 ]; then
        exit $?
    fi

    return 0
}

# Vertex Shaders
shadercompile vs_main vertex

# Fragment Shaders
shadercompile fs_main fragment

exit 0