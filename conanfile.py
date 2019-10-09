from conans import ConanFile, CMake


class CPictureBench(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = [("celero/2.5.0@ppodsiadly/stable", )]
    generators = "cmake"
    default_options = {}

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")  # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib")  # From lib to bin

