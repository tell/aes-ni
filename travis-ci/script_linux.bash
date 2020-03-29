#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; mkdir -p build; cd build; cmake -DCMAKE_BUILD_TYPE=Release ..; make -j"
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT/build; LSAN_OPTIONS=verbosity=1:log_threads=1 ctest --verbose"
