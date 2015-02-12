
mkdir build-opencv; cd build-opencv
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../deps -DWITH_IPP=ON -DWITH_TBB=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF ../3rdparty/opencv
make -j && make install
cd ..

mkdir build-flatbuffers; cd build-flatbuffers
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../deps -DFLATBUFFERS_BUILD_TESTS=OFF ../3rdparty/flatbuffers
ninja && ninja install
cd ..

mkdir build-cppmicroservices; cd build-cppmicroservices
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../deps ../3rdparty/CppMicroServices
ninja && ninja install
cd ..
