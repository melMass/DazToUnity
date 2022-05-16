
set -e
# RM -rf _build
conan source . --source-folder=_build/source
conan install . --install-folder=_build/build/ --build=missing -s build_type=Release --profile ./profiles/osx-default.profile
conan build . --source-folder=_build/source --build-folder=_build/build
cmake --install _build/build
