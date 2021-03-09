set -e
cd build
cmake -DCMAKE_OSX_SYSROOT=/Developer/OSX_sdks/MacOSX10.6.sdk -DCMAKE_OSX_ARCHITECTURES="x86_64" -DDAZ_STUDIO_EXE_DIR="/Users/imac/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit" ..
make
