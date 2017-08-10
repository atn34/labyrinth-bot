#! /bin/bash

set -euxo pipefail

# opencv

if ! pkg-config --exists opencv ; then 
    wget https://github.com/opencv/opencv/archive/3.2.0.tar.gz -O opencv-3.2.0.tar.gz
    tar -xvf opencv-3.2.0.tar.gz
    pushd opencv-3.2.0
    mkdir build
    pushd build
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
    make -j8
    make install
    popd
    popd
fi

# gtest

cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
