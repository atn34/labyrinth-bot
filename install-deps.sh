#! /bin/bash

set -euxo pipefail

# opencv

OPENCV_VERSION=3.3.0
OPENCV_TARBALL=opencv-${OPENCV_VERSION}.tar.gz

wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.tar.gz -O opencv-${OPENCV_VERSION}.tar.gz
tar -xvf ${OPENCV_TARBALL}
rm ${OPENCV_TARBALL}
pushd opencv-${OPENCV_VERSION}
mkdir build
pushd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j8
sudo make install
popd
popd
rm -rf opencv-${OPENCV_VERSION}

# googletest

GOOGLETEST_VERSION=1.8.0
GOOGLETEST_TARBALL=googletest-${GOOGLETEST_VERSION}.tar.gz
wget https://github.com/google/googletest/archive/release-${GOOGLETEST_VERSION}.tar.gz -O ${GOOGLETEST_TARBALL}
tar -xvf ${GOOGLETEST_TARBALL}
rm ${GOOGLETEST_TARBALL}
pushd googletest-release-${GOOGLETEST_VERSION}
mkdir -p build
pushd build
cmake ..
make -j8
sudo make install
popd
popd
rm -rf googletest-release-${GOOGLETEST_VERSION}

# libsocket

LIBSOCKET_VERSION=2.4.1
LIBSOCKET_TARNAME=libsocket-${LIBSOCKET_VERSION}.tar.gz

wget https://github.com/dermesser/libsocket/archive/v${LIBSOCKET_VERSION}.tar.gz -O ${LIBSOCKET_TARNAME}
tar -xvf ${LIBSOCKET_TARNAME}
rm ${LIBSOCKET_TARNAME}
pushd libsocket-${LIBSOCKET_VERSION}
mkdir -p build
pushd build
cmake ..
make -j8
sudo make install
popd
popd
rm libsocket-${LIBSOCKET_VERSION}

