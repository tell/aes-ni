#!/bin/bash
set -ev
docker exec -ti ${MY_NAME} bash -c "apt update -q" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt upgrade -q -y" > /dev/null
docker exec -ti ${MY_NAME} bash -c "DEBIAN_FRONTEND=noninteractive apt install -q -y build-essential git cmake libgmp-dev" > /dev/null

docker exec -ti ${MY_NAME} bash -c "git clone --depth 1 https://github.com/fmtlib/fmt.git ${MY_TARGET_MOUNT}/third_party/fmt"
#docker exec -ti ${MY_NAME} bash -c "cd fmt; mkdir -p build; cd build; cmake ..; make -j; make install"

docker exec -ti ${MY_NAME} bash -c "apt install -q -y libspdlog-dev"

docker exec -ti ${MY_NAME} bash -c "git clone --depth 1 https://github.com/google/googletest.git"
docker exec -ti ${MY_NAME} bash -c "cd googletest; mkdir -p build; cd build; cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr; make -j; make install; ldconfig"
