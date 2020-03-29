#!/bin/bash
set -ev
alias docw="docker exec -ti ${MY_NAME} bash -c"
docker exec -ti ${MY_NAME} bash -c "apt update" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt upgrade -y" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt install -y build-essential git cmake" > /dev/null

docker exec -ti ${MY_NAME} bash -c "git clone https://github.com/fmtlib/fmt.git"
docker exec -ti ${MY_NAME} bash -c "cd fmt; mkdir -p build; cd build; cmake ..; make -j; make install"

docker exec -ti ${MY_NAME} bash -c "git clone https://github.com/google/googletest.git"
docker exec -ti ${MY_NAME} bash -c "cd googletest; mkdir -p build; cd build; cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr; make -j; make install; ldconfig"
