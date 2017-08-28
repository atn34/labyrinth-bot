#! /bin/bash

set -euxo pipefail

# opencv

OPENCV_VERSION=3.3.0
OPENCV_TARBALL=opencv-${OPENCV_VERSION}.tar.gz
OPENCV_PREBUILT=opencv-${OPENCV_VERSION}-prebuilt

wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.tar.gz -O ${OPENCV_TARBALL}
tar -xvf ${OPENCV_TARBALL}
rm ${OPENCV_TARBALL}
pushd opencv-${OPENCV_VERSION}
mkdir -p build
pushd build
mkdir -p ${OPENCV_PREBUILT}
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=./${OPENCV_PREBUILT} ..
make -j8
make install
tar -cvzf ../../${OPENCV_PREBUILT}.tar.gz ${OPENCV_PREBUILT}
popd
popd
rm -rf opencv-${OPENCV_VERSION}

