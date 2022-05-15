$current_pwd = $PWD

$parent_dir = Split-Path $MyInvocation.MyCommand.Path

Set-Location $parent_dir

New-Item -ErrorAction Ignore -ItemType Directory build
Set-Location build
cmake .. -DCMAKE_BUILD_TYPE=Release -DDAZ_STUDIO_EXE_DIR="C:\Daz 3D\Applications\64-bit\DAZ 3D\DAZStudio4\"
cmake --build .
cmake --install .


Set-Location $current_pwd
