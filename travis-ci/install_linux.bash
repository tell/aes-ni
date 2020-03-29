#!/bin/bash
set -ev
alias docw="docker exec -ti ${MY_NAME} bash -c"
docw "apt update" > /dev/null
docw "apt upgrade -y" > /dev/null
docw "apt install -y build-essential git cmake libfmt-dev libgtest-dev"
docw "pwd"
docw "git clone https://github.com/google/googletest.git"
docw "cd googletest; mkdir build && cd build; cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr; make -j8; sudo make install; sudo ldconfig"
