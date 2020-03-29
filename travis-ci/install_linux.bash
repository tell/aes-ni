#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "apt update" > /dev/null
docker exec -ti $MY_NAME bash -c "apt install -y build-essential cmake g++-9" > /dev/null
