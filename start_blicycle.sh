#!/bin/bash

echo "Starting Blicycle computer vision..."
CV_MODE=capture
CV_PARAM=1
YEI_SOURCE=/dev/ttyACM1
HOKUYO_SOURCE=/dev/ttyACM0
#./blicycle_cv/build/blicycle $CV_MODE $CV_PARAM
~/BlicycleRetry/software/build/bin/yei_3_space_sensor $YEI_SOURCE
~/BlicycleRetry/software/build/bin/hokuyo $HOKUYO_SOURCE

echo "...done!"
