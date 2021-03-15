# Daz To Unity [EXPERIMENTAL]

This is a fork of the [official](https://github.com/daz3d/DazToRuntime/) open sourced version.

## Features
- **Genesis 8.1 Support**: FACS Morphs 
- **Support Subdivisions**: By default when you export a subdivided figure to FBX you loose half of the skin weights.
- **HDRP 10 Shaders**: Converting Daz shaders automatically.
- **Blendshape Controls**: Very crude implementation for now. Allows you to control the Blendshapes (Morphs) of multiple items at once.
- **Driver**: JCM (Corrective Morphs) Support *For now this is part of a private repo, will come soon* 

## Build os OSX
```bash
git clone https://github.com/melMass/DazToUnity.git
cd DazToUnity
mkdir -p build
cd build
cmake -DCMAKE_OSX_SYSROOT=/Developer/OSX_sdks/MacOSX10.7.sdk -DCMAKE_OSX_ARCHITECTURES="x86_64" -DDAZ_STUDIO_EXE_DIR="/Users/$(whoami)/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit" ..
# Then run daz
```