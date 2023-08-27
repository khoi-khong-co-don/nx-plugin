from conans import ConanFile, CMake

class OryzaPluginConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    
    options = {"shared": [True, False]}
    
    default_options = {"shared": False}
    
    generators = "cmake", "cmake_find_package", "virtualrunenv"

    requires = "boost/1.78.0"
    def configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["boost"]
