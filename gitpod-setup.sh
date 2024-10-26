mkdir lib
cd lib
wget https://github.com/xpack-dev-tools/gcc-xpack/releases/download/v14.2.0-1/xpack-gcc-14.2.0-1-linux-x64.tar.gz
tar -xvzf xpack-gcc-14.2.0-1-linux-x64.tar.gz
cd ..

mkdir build
cd build

cmake -DCMAKE_C_COMPILER=/workspace/ConnectFour/lib/xpack-gcc-14.2.0-1/bin/gcc -DCMAKE_CXX_COMPILER=/workspace/ConnectFour/lib/xpack-gcc-14.2.0-1/bin/g++ -DCMAKE_BUILD_TYPE=Release ..

cd ..