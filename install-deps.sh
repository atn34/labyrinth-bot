#! /bin/bash

set -euxo pipefail

# opencv

OPENCV_VERSION=3.3.0

if ! pkg-config --exists opencv ; then 
    wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.tar.gz -O opencv-${OPENCV_VERSION}.tar.gz
    tar -xvf opencv-${OPENCV_VERSION}.tar.gz
    pushd opencv-${OPENCV_VERSION}
    mkdir build
    pushd build
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
    make -j8
    sudo make install
    popd
    popd
fi

# gtest

pushd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
popd
