#!/bin/bash -e
sudo apt-get install build-essential texinfo bison flex libgmp3-dev libmpfr-dev libmpc-dev

mkdir -p crosscompiler && cd crosscompiler
PREFIX=`pwd`
echo PREFIX=$PREFIX

# Download binutils-2.27
if [ ! -f binutils-2.27.tar.gz ]; then
    wget http://ftp.gnu.org/gnu/binutils/binutils-2.27.tar.gz
    tar xvf binutils-2.27.tar.gz
fi

# Configure and build binutils
#cd binutils-2.27
CC=/usr/bin/gcc-5
CXX=/usr/bin/g++-5
CPP=/usr/bin/cpp-7
LD=/usr/bin/gcc-5
#mkdir -p build && cd build
#../configure --disable-nls --target=i386-elf --disable-werror --enable-gold=yes --prefix=$PREFIX
#make all
#make install

cd $PREFIX
pwd

# Download gcc-7.10
if [ ! -f gcc-7.1.0.tar.bz2 ]; then
    wget https://ftp.gnu.org/gnu/gcc/gcc-7.1.0/gcc-7.1.0.tar.bz2
    bunzip2 gcc-7.1.0.tar.bz2
    tar xvf gcc-7.1.0.tar
    rm gcc-7.1.0.tar
fi

# Configure and build gcc-7
PATH=$PATH:$PREFIX/bin
cd gcc-7.1.0
mkdir -p build && cd build
../configure --disable-nls --target=i386-elf --disable-werror --prefix=$PREFIX --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc
echo "Linking to /usr/local/bin"
ln -s $PREFIX/bin/i386-elf-g++ /usr/local/bin/i386-elf-g++
