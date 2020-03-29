#!/bin/bash
set -ev
docker exec -ti $MY_NAME bash -c "apt update"
docker exec -ti $MY_NAME bash -c "apt install -y build-essential cmake g++-9"
