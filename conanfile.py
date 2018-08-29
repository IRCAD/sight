from conans import ConanFile, CMake, tools

class Fw4splConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    default_options = "boost:shared=True"

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="lib", src="lib")
        self.copy("*.so*", dst="lib", src="lib")

    def requirements(self):
        if tools.os_info.is_windows:
            self.requires("winiconv/0.0.8@fw4spl/stable")
        if tools.os_info.is_windows or tools.os_info.is_macos:
            self.requires("libxml2/2.9.8@fw4spl/stable")

        self.requires("boost/1.67.0@fw4spl/stable")
        self.requires("camp/0.8.2@fw4spl/stable")
        self.requires("glm/0.9.8.5@fw4spl/stable")
        self.requires("qt/5.11.1@fw4spl/stable")
        self.requires("cppunit/1.14.0@fw4spl/stable")
        self.requires("vtk/8.0.1@fw4spl/stable")
        self.requires("gdcm/2.8.6@fw4spl/stable")
        self.requires("itk/4.13.0@fw4spl/stable")
        self.requires("dcmtk/3.6.3@fw4spl/stable")
