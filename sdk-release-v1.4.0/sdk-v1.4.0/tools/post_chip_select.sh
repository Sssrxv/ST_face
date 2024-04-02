#!/usr/bin/env bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
# echo "${SCRIPT_PATH}"

CONFIG_FILE=$1
SDK_FOLDER=$2
# echo "config file:${CONFIG_FILE}"
# echo "lib folder:${SDK_FOLDER}"

pushd ${SCRIPT_PATH} > /dev/null
CHIP=$(cat ${CONFIG_FILE} | grep "CONFIG_CHIP" | grep "=y" | sed -r 's/CONFIG_CHIP(.*)=y/\1/g' | awk '{print tolower($0)}')
echo "selected chip is:${CHIP}"

if [ ! -d "${SDK_FOLDER}" ]; then
    echo "sdk folder not exist, sdk post process script return"
    exit 0
fi

# change softlink of libai31xxsdk.a
pushd ${SDK_FOLDER}/lib > /dev/null
if [[ "$CHIP" == *"h264"* ]]; then
    TARGET_SDK_LIB=`ls | grep ${CHIP}_ `
else
    TARGET_SDK_LIB=`ls | grep ${CHIP}_sdk `
fi
# TARGET_SDK_LIB=`ls | grep ${CHIP}_`
TARGET_SDK_LINK=libai31xxsdk.a
if [ -e "${TARGET_SDK_LINK}" ]; then
    rm ${TARGET_SDK_LINK}
fi
ln -s ${TARGET_SDK_LIB} ${TARGET_SDK_LINK}
popd > /dev/null

# change softlink of link script
pushd ${SDK_FOLDER}/ldscript > /dev/null
TARGET_LINK_SCRIPT=`ls | grep ${CHIP}.ld`
TARGET_LD_LINK=ai31xx.ld
if [ -e "${TARGET_LD_LINK}" ]; then
    rm ${TARGET_LD_LINK}
fi
ln -s ${TARGET_LINK_SCRIPT} ${TARGET_LD_LINK}
popd > /dev/null

# change softlink of libflags
pushd ${SDK_FOLDER}/scripts > /dev/null
if [[ "$CHIP" == *"h264"* ]]; then
    TARGET_LIB_FLAGS=`ls | grep ${CHIP}_ | grep libflags`
else
    TARGET_LIB_FLAGS=`ls | grep ${CHIP}_Makefile | grep libflags`
fi
# echo "TARGET_LIB_FLAGS: " ${TARGET_LIB_FLAGS}
# TARGET_LIB_FLAGS=`ls | grep ${CHIP}_ | grep libflags`
TARGET_LIB_FLAGS_LINK=Makefile.libflags
if [ -e "${TARGET_LIB_FLAGS_LINK}" ]; then
    rm ${TARGET_LIB_FLAGS_LINK}
fi
ln -s ${TARGET_LIB_FLAGS} ${TARGET_LIB_FLAGS_LINK}
popd > /dev/null

# change softlink of libincs
pushd ${SDK_FOLDER}/scripts > /dev/null
if [[ "$CHIP" == *"h264"* ]]; then
    TARGET_LIB_INCS=`ls | grep ${CHIP}_ | grep libincs`
else
    TARGET_LIB_INCS=`ls | grep ${CHIP}_Makefile | grep libincs`
fi
# TARGET_LIB_INCS=`ls | grep ${CHIP}_ | grep libincs`
TARGET_LIB_INCS_LINK=Makefile.libincs
if [ -e "${TARGET_LIB_INCS_LINK}" ]; then
    rm ${TARGET_LIB_INCS_LINK}
fi
ln -s ${TARGET_LIB_INCS} ${TARGET_LIB_INCS_LINK}
popd > /dev/null

# change softlink of spl
pushd ${SDK_FOLDER}/tools/bin2rom/spl > /dev/null
if [[ "${CHIP}" == *"xh"* ]]; then
    TARGET_SPL=`ls | grep ai31xxh_nor_spl`
else
    TARGET_SPL=`ls | grep ai31xx_nor_spl`
fi


TARGET_SPL_LINK=nor_spl.img
if [ -e "${TARGET_SPL_LINK}" ]; then
    rm ${TARGET_SPL_LINK}
fi
ln -s ${TARGET_SPL} ${TARGET_SPL_LINK}
popd > /dev/null


popd > /dev/null
