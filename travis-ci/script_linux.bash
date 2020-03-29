#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; mkdir build; cd build; cmake ..; make"
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT/build; ctest"
