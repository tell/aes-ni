#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "$CC --version"
docker exec -ti $MY_NAME bash -c "$CXX --version"
