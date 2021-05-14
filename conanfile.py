from conans import ConanFile, CMake

class DazConan(ConanFile):
    name = "daz-runtime"
    version = "1.1.0"
    license = "LICENSE-2.0"
    author = "Mel Massadian melmassadian@gmail.com"
    url = "https://github.com/melMass/DazToUnity"
    description = "DAZ to Game Engines: Unity (URP + HDRP 10) / Unreal 4.26"
    topics = ("daz", "plugin")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    exports_sources = [
      "Common/*",
      "Unity/DazStudioPlugin/*",
      "Unity/CMakeLists.txt",
      "CMakeLists.txt",
      "Unreal/DazStudioPlugin/*",
      "Unreal/CMakeLists.txt",
      "SDK/*",
      "LICENSE"

      ]
    # USE DAZ BUNDLED ONE INSTEAD.....!!!!
    # requires=["fbxsdk/2020.1.1"]
    # requires=["fbxsdk/2014.2.1"]

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

        self.copy("*.h", dst="include", src="src")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("LICENSE")

    def package_info(self):
        self.cpp_info.libs = ["daz-runtime-unity","daz-runtime-unreal"]
