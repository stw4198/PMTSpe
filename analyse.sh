#!/bin/bash

echo " -------------------------------"
date  
echo "running"
echo "analyse.sh"
echo " -------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )

FILE_PATH=${DIR_PATH}${FILE_NAME}

${WM_CONVERT}/dat_to_root ${FILE_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"

mkdir -p ./Plots/DAQ/

${WM_COOK}/cook_raw ${FILE_PATH}.root

echo " ------------------------------"
date 
echo " ------------------------------"

${WM_DARK}/dark ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"