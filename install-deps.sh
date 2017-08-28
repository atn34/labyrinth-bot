#! /bin/bash

set -euxo pipefail

# opencv

OPENCV_VERSION=3.3.0
OPENCV_PREBUILT_TARBALL=opencv-${OPENCV_VERSION}-prebuilt.tar.gz

wget https://www.dropbox.com/s/eio2o4xajg605dx/opencv-3.3.0-prebuilt.tar.gz?dl=0 -O ${OPENCV_PREBUILT_TARBALL}
tar -xvf ${OPENCV_PREBUILT_TARBALL}
rm ${OPENCV_PREBUILT_TARBALL}

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
LIBSOCKET_TARBALL=libsocket-${LIBSOCKET_VERSION}.tar.gz

wget https://github.com/dermesser/libsocket/archive/v${LIBSOCKET_VERSION}.tar.gz -O ${LIBSOCKET_TARBALL}
tar -xvf ${LIBSOCKET_TARBALL}
rm ${LIBSOCKET_TARBALL}
pushd libsocket-${LIBSOCKET_VERSION}
mkdir -p build
pushd build
cmake ..
make -j8
sudo make install
popd
popd
rm libsocket-${LIBSOCKET_VERSION}

