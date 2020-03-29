#!/bin/bash
set -ev
alias docw="docker exec -ti ${MY_NAME} bash -c"
docker exec -ti ${MY_NAME} bash -c "apt update" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt upgrade -y" > /dev/null
docker exec -ti ${MY_NAME} bash -c "apt install -y build-essential git cmake libfmt-dev libgtest-dev"
docker exec -ti ${MY_NAME} bash -c "pwd"
docker exec -ti ${MY_NAME} bash -c "git clone https://github.com/google/googletest.git"
docker exec -ti ${MY_NAME} bash -c "cd googletest; mkdir build && cd build; cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr; make -j8; make install; ldconfig"
