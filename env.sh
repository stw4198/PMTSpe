#!/bin/bash

echo "Setting PMT Testing Environment as:"

# pmt testing code
export WM_CODE="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
echo ${WM_CODE}

export WM_CONVERT=${WM_CODE}/Binary_Conversion/
export WM_COOK=${WM_CODE}/Cooking/
export WM_DARK=${WM_CODE}/Dark/
export WM_COMMON=${WM_CODE}/Common_Tools/

# headers
export CPATH=${CPATH}:${WM_COMMON}
export CPATH=${CPATH}:${WM_COOK}

# binaries
export PATH=${PATH}:${WM_CODE}
export PATH=${PATH}:${WM_CONVERT}
export PATH=${PATH}:${WM_COOK}
export PATH=${PATH}:${WM_DARK}

# libraries
if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COOK}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COMMON}
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COOK}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COMMON}
fi
