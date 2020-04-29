#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; mkdir -p build; cd build; cmake -DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Release ..; make"
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT/build; ctest --verbose"
