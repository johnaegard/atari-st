#!/bin/bash -x

sudo add-apt-repository -y ppa:vriviere/ppa
sudo apt update
sudo apt install -y gcc cmake libsdl2-dev zlib1g-dev libpng-dev libreadline-dev bzip2 cross-mint-essential
wget --no-clobber https://framagit.org/hatari/releases/-/raw/main/v2.5/hatari-2.5.0.tar.bz2
bunzip2 hatari-2.5.0.tar.bz2
tar -xvf hatari-2.5.0.tar
cd hatari-2.5.0 
mkdir -p build
cd build
cmake ..
cmake --build . -j`getconf _NPROCESSORS_ONLN`
cd ../../../bin
rm * 
ln -s ../.hatari/hatari-2.5.0/build/src/hatari
ln -s ../.hatari/hatari-2.5.0/tools/debugger/*.py .
ln -s ../.hatari/hatari-2.5.0/build/tools/debugger/gst2ascii
echo "cmake --install ." to install systemwide.
