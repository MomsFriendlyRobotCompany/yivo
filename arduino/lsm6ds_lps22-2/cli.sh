#!/bin/bash

USEAGE="Usage: {$0} make|[upload port]"
FQBN="adafruit:samd:adafruit_qtpy_m0"

if [[ $# == "0" ]]; then
    echo ${USEAGE}
fi

if [[ $1 == "make" ]]; then
    echo ">> Make"
    arduino-cli compile --fqbn ${FQBN} .
elif [[ $1 == "upload" && $# == "2" ]]; then
    echo ">> Uploading to $2"
    arduino-cli upload --port $2 --fqbn ${FQBN} .
else
    echo ${USEAGE}
fi
