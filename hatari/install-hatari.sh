#!/bin/bash -x

sudo apt install -y gcc cmake libsdl2-dev zlib1g-dev libpng-dev libreadline-dev
wget --no-clobber https://git.tuxfamily.org/hatari/hatari.git/snapshot/hatari-2.5.0.tar.gz
tar -xvzf hatari-2.5.0.tar.gz
cd hatari-2.5.0 
mkdir -p build
cd build
cmake ..
cmake --build . -j`getconf _NPROCESSORS_ONLN`

echo "cmake --install ." to install.
