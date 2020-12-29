#!/bin/bash
set -ev
docker exec -ti ${MY_NAME} bash -c "apt-get update -q" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt-get upgrade -q -y" > /dev/null
docker exec -ti ${MY_NAME} bash -c "env DEBIAN_FRONTEND=noninteractive apt-get install -q -y build-essential git cmake libgmp-dev"

docker exec -ti ${MY_NAME} bash -c "env DEBIAN_FRONTEND=noninteractive apt-get install -q -y libfmt-dev"
#docker exec -ti ${MY_NAME} bash -c "git clone https://github.com/fmtlib/fmt.git -b 6.1.2 --depth 1 ${MY_TARGET_MOUNT}/third_party/fmt"
#docker exec -ti ${MY_NAME} bash -c "cd fmt; mkdir -p build; cd build; cmake ..; make -j; make install"

docker exec -ti ${MY_NAME} bash -c "env DEBIAN_FRONTEND=noninteractive apt-get install -q -y libspdlog-dev libgtest-dev"

#docker exec -ti ${MY_NAME} bash -c "git clone https://github.com/google/googletest.git --depth 1"
#docker exec -ti ${MY_NAME} bash -c "cd googletest; mkdir -p build; cd build; cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr; make -j; make install; ldconfig"
