#Move to the build directory
mkdir build
cd build

#Create the build files
cmake ..

#Compile and generate the u2f file
make -j$(nproc)