cmake_minimum_required(VERSION 3.19)

# Use new policy for 'install_name' and RPATH on macOS (use `cmake --help-policy CMP0068` for details)
cmake_policy(SET CMP0068 NEW)
# Use new policy for `FindOpenGL` to prefer GLVND by default when available on linux.
# (use `cmake --help-policy CMP0072` for details).
cmake_policy(SET CMP0072 NEW)
# Use new policy for 'Honor visibility properties for all target types'.
# (use `cmake --help-policy CMP0063` for details)
cmake_policy(SET CMP0063 NEW)
# Adds support for the new IN_LIST operator.
cmake_policy(SET CMP0057 NEW)
# Use new policy to use CMAKE_CXX_STANDARD in try_compile() macro
cmake_policy(SET CMP0067 NEW)
# Use old policy for Documentation to add cache variables and find VTK documentation dependent packages.
# (Needed for doxygen..)
cmake_policy(SET CMP0106 OLD)

if(${CMAKE_VERSION} VERSION_GREATER "3.20.0")
    # Transform the ``DEPFILE`` after running the custom command for Ninja generator.
    cmake_policy(SET CMP0116 OLD)
endif()

# On Windows, if the user doesn't specify a value,
# 'CMAKE_BUILD_TYPE' is automatically initialized to 'Debug' after 'project()'.
# So we need to check this variable at this point.
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
                                               "Choose the type of build, options are: Debug, Release, RelWithDebInfo"
)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL
                                                                                         "RelWithDebInfo"
)
    message(
        FATAL_ERROR
            "Invalid value for CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}' (required Debug, Release, RelWithDebInfo)"
    )
endif()

project(
    sight
    VERSION 22.0.0
    DESCRIPTION "Surgical Image Guidance and Healthcare Toolkit"
    HOMEPAGE_URL "https://git.ircad.fr/sight/sight"
)

if(APPLE)
    message(FATAL_ERROR "macOS is not supported.")
endif()

enable_testing()

include(CheckVariableExists)
include(CMakeParseArguments)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(SIGHT_ENABLE_PCH "Use pre-compiled headers to speedup the compilation" ON)
option(SIGHT_VERBOSE_PCH "Display debug messages to help debugging PCH" OFF)
mark_as_advanced(SIGHT_ENABLE_PCH)
mark_as_advanced(SIGHT_VERBOSE_PCH)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/macros.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/pch.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/linux/modules)

# We find VTK to know its version so we can enable PCL or not
find_package(VTK CONFIG QUIET REQUIRED)

########################################################################################################################
# User options
########################################################################################################################

# Tests build / run options
option(SIGHT_BUILD_TESTS "Configures projects associated tests (<project>Test projects)" ON)

option(SIGHT_TESTS_XML_OUTPUT "Tests will generate an xml output, suitable for CI integration" ON)
mark_as_advanced(SIGHT_TESTS_XML_OUTPUT)

set(SIGHT_TESTS_FILTER "" CACHE STRING "Allows to only build/run tests whose path contains the filter string.")
mark_as_advanced(SIGHT_TESTS_FILTER)

if(UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
    option(SIGHT_ENABLE_GDB
           "Test scripts will attach GDB, allowing to generate core file and print backtrace in case of crash." OFF
    )
    mark_as_advanced(SIGHT_ENABLE_GDB)

    if(SIGHT_ENABLE_GDB)
        find_program(GDB NAMES "gdb" REQUIRED)
    endif()
endif()

# Use clang-tidy linter
option(SIGHT_ENABLE_CLANG_TIDY "Enable Clang Tidy checks" OFF)
mark_as_advanced(SIGHT_ENABLE_CLANG_TIDY)

if(SIGHT_ENABLE_CLANG_TIDY)
    # cmake-lint: disable=C0301
    find_program(CLANG_TIDY NAMES "clang-tidy" "clang-tidy-14" REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY};--use-color;--export-fixes=.clang-tidy-fixes.yaml)

    # Also generate compile_commands.json which is useful when launching clang-tidy by "hand"
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# QML_IMPORT_PATH allows qtCreator to find the qml modules created in our modules
set(QML_IMPORT_PATH "" CACHE STRING "Path of the Qml modules." FORCE)
mark_as_advanced(QML_IMPORT_PATH)

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "List of supported configurations." FORCE)
endif()

# Allow to use an encrypted log
option(SIGHT_ENABLE_ENCRYPTED_LOG "Allow log encryption" OFF)

# Defines the default hardcoded password. The password itself will be obfuscated during build process, it will not
# appear in "clear" text in the final binary.
set(SIGHT_DEFAULT_PASSWORD "" CACHE STRING "Default password to use for encryption (log, preferences, session, ...).")
mark_as_advanced(SIGHT_DEFAULT_PASSWORD)

# Add global compile definitions because of precompiled headers
if(SIGHT_ENABLE_ENCRYPTED_LOG)
    add_compile_definitions(SIGHT_ENABLE_ENCRYPTED_LOG)
endif()
if(SIGHT_DEFAULT_PASSWORD)
    add_compile_definitions(SIGHT_DEFAULT_PASSWORD="${SIGHT_DEFAULT_PASSWORD}")
endif()

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER automoc)

# Make openvslam optional on Linux
if(NOT MSVC)
    option(SIGHT_ENABLE_OPENVSLAM "Enable OpenVSLAM" OFF)
endif()
# Make Realsense optional
option(SIGHT_ENABLE_REALSENSE "Enable RealSense" OFF)

# Make samples/tutorials build optional
option(SIGHT_BUILD_EXAMPLES "Build examples and tutorials" ON)

########################################################################################################################
# Warn user of install dir isn't empty
########################################################################################################################

file(GLOB_RECURSE INSTALL_DIR_CONTENT ${CMAKE_INSTALL_PREFIX}/*)
list(LENGTH INSTALL_DIR_CONTENT CONTENT)
if(NOT CONTENT EQUAL 0)
    # DIR isn't empty, warn user.
    message(WARNING "CMAKE_INSTALL_PREFIX (${CMAKE_INSTALL_PREFIX}) isn't empty."
                    "Please select another folder or clean it before running install command."
    )
endif()

########################################################################################################################
# External libraries management
########################################################################################################################

set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

# Append our 'FindPackages.cmake' to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/modules)

if(UNIX)
    list(APPEND CMAKE_PREFIX_PATH ${FWCMAKE_RESOURCE_PATH}/modules)

    list(APPEND CMAKE_FIND_ROOT_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    if(UNIX)
        list(APPEND CMAKE_PREFIX_PATH /usr/share/OGRE/cmake/modules)
    endif()

    set(SIGHT_EXTERNAL_LIBRARIES CACHE PATH "External libraries location")
    mark_as_advanced(SIGHT_EXTERNAL_LIBRARIES)

    # Use directly SIGHT_EXTERNAL_LIBRARIES if set, otherwise, download and install sight-deps package
    # OpenVSLAM is the only package provided by sight-deps
    if(NOT SIGHT_EXTERNAL_LIBRARIES AND SIGHT_ENABLE_OPENVSLAM)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/download_deps.cmake)
    endif()

    if(SIGHT_EXTERNAL_LIBRARIES)
        get_filename_component(ABSOLUTE_SIGHT_EXTERNAL_LIBRARIES ${SIGHT_EXTERNAL_LIBRARIES} REALPATH)
        set(SIGHT_EXTERNAL_LIBRARIES ${ABSOLUTE_SIGHT_EXTERNAL_LIBRARIES})
        unset(ABSOLUTE_SIGHT_EXTERNAL_LIBRARIES)

        list(APPEND CMAKE_PREFIX_PATH ${SIGHT_EXTERNAL_LIBRARIES})
        list(APPEND CMAKE_MODULE_PATH ${SIGHT_EXTERNAL_LIBRARIES}/lib/cmake/)
        list(APPEND CMAKE_FIND_ROOT_PATH ${SIGHT_EXTERNAL_LIBRARIES})
        # To be added into LD_LIBRARY_PATH in our "launch" scripts.
        set(SIGHT_EXTERNAL_LIBRARIES_LIB_PATH ${SIGHT_EXTERNAL_LIBRARIES}/lib/)
    endif()
endif()

# We always need CppUnit, no need to put that in subdirectories
find_package(CppUnit QUIET REQUIRED)

########################################################################################################################
# Default paths settings for libraries, modules and resources
########################################################################################################################

set(FW_INSTALL_PATH_SUFFIX "${PROJECT_NAME}")
set(SIGHT_MODULE_RC_PREFIX "${CMAKE_INSTALL_DATADIR}/${FW_INSTALL_PATH_SUFFIX}")
if(WIN32)
    set(SIGHT_MODULE_LIB_PREFIX "${CMAKE_INSTALL_BINDIR}")
else()
    set(SIGHT_MODULE_LIB_PREFIX "${CMAKE_INSTALL_LIBDIR}")
endif()

set(FWCONFIG_PACKAGE_LOCATION lib/cmake/sight)
set_property(GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS "")

# Define the path 'FW_SIGHT_EXTERNAL_LIBRARIES_DIR' used to find external libraries required by our applications
set_external_libraries_dir()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(SIGHT_VCPKG_ROOT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug")
else()
    set(SIGHT_VCPKG_ROOT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    set(EXCLUDE_PATTERN ".*/debug/.*")
endif()

add_subdirectory(libs)
add_subdirectory(modules)
add_subdirectory(configs)
add_subdirectory(activities)
add_subdirectory(utils)
add_subdirectory(apps)
if(SIGHT_BUILD_EXAMPLES)
    add_subdirectory(tutorials)
    add_subdirectory(examples)
endif()
add_subdirectory(3rd-party)

########################################################################################################################
# Export and install targets
########################################################################################################################

# Will export COMPONENTS: list of all available components ordered by dependency (no dependency first).
sight_generate_component_list(COMPONENTS)

# Create the sightConfigVersion file
configure_file(${CMAKE_SOURCE_DIR}/cmake/build/sightConfig.cmake.in ${CMAKE_BINARY_DIR}/cmake/sightConfig.cmake @ONLY)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/cmake/sightConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY AnyNewerVersion
)

# Install a special xvfb-run wrapper script to workaround bugs withe the system xvfb-run launcher script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/linux/safe-xvfb-run ${CMAKE_CURRENT_BINARY_DIR}/bin/safe-xvfb-run
    COPYONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/linux/exec_gui_tests.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/exec_gui_tests.sh
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/windows/exec_gui_tests.bat
    ${CMAKE_CURRENT_BINARY_DIR}/bin/exec_gui_tests.bat COPYONLY
)

# Install the sightConfig.cmake and sightConfigVersion.cmake
install(
    FILES "${CMAKE_BINARY_DIR}/cmake/sightConfig.cmake" "${CMAKE_BINARY_DIR}/cmake/sightConfigVersion.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/macros.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/linux/modules/FindFilesystem.cmake"
    DESTINATION ${FWCONFIG_PACKAGE_LOCATION}
    COMPONENT dev
)

# install CppUnitConfig.cmake in a modules folder.
install(FILES "${CMAKE_SOURCE_DIR}/cmake/modules/CppUnitConfig.cmake" DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/modules
        COMPONENT dev
)

# Install some files needed for the build
install(
    FILES "${CMAKE_SOURCE_DIR}/cmake/build/configure_file.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/flags.cmake"
          "${FWCMAKE_RESOURCE_PATH}/build/cppunit_main.cpp"
          "${CMAKE_SOURCE_DIR}/cmake/build/config.hpp.in"
          "${CMAKE_SOURCE_DIR}/cmake/build/plugin_config.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/plugin_config_command.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/profile_config.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/build/profile.xml.in"
          "${CMAKE_SOURCE_DIR}/cmake/build/registerServices.cpp.in"
    DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/build
    COMPONENT dev
)

# Install some files needed for the install
install(
    FILES "${CMAKE_SOURCE_DIR}/cmake/install/generic_install.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/install/get_git_rev.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/install/helper.cmake"
          "${CMAKE_SOURCE_DIR}/cmake/install/install_imported.cmake.in"
          "${CMAKE_SOURCE_DIR}/cmake/install/pre_package.cmake"
    DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install
    COMPONENT dev
)

if(WIN32)
    install(FILES "${CMAKE_SOURCE_DIR}/cmake/build/windows/template_exe.bat.in"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/build/windows COMPONENT dev
    )
    install(
        FILES "${CMAKE_SOURCE_DIR}/cmake/install/windows/package.cmake"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/install_plugins.cmake"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/template.bat.in"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/setpath.bat.in"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/windows_fixup.cmake.in"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/generate_headers.cmake"
        DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/windows
        COMPONENT dev
    )
    install(FILES "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/NSIS.InstallOptions.ini.in"
                  "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/NSIS.template.in"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/
    )
    install(
        FILES "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/banner_nsis.bmp"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/dialog_nsis.bmp"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/app.ico"
              "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/license.rtf"
        DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/rc/
    )
    install(PROGRAMS "${CMAKE_SOURCE_DIR}/cmake/install/windows/exec_gui_tests.bat"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/windows COMPONENT dev
    )
elseif(UNIX)
    install(
        FILES "${CMAKE_SOURCE_DIR}/cmake/build/linux/template.sh.in"
              "${CMAKE_SOURCE_DIR}/cmake/build/linux/template_exe.sh.in"
              "${CMAKE_SOURCE_DIR}/cmake/build/linux/template_test.sh.in"
        DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/build/linux
        COMPONENT dev
    )
    install(FILES "${CMAKE_SOURCE_DIR}/cmake/install/linux/package.cmake"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/linux COMPONENT dev
    )

    install(PROGRAMS "${CMAKE_SOURCE_DIR}/cmake/install/linux/safe-xvfb-run"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/linux COMPONENT dev
    )

    install(PROGRAMS "${CMAKE_SOURCE_DIR}/cmake/install/linux/exec_gui_tests.sh"
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/linux COMPONENT dev
    )

    install(
        FILES "${CMAKE_SOURCE_DIR}/cmake/install/linux/template.sh.in"
              "${CMAKE_SOURCE_DIR}/cmake/install/linux/template_exe.sh.in"
              "${CMAKE_SOURCE_DIR}/cmake/install/linux/linux_fixup.cmake.in"
        DESTINATION ${FWCONFIG_PACKAGE_LOCATION}/install/linux
        COMPONENT dev
    )
endif()

########################################################################################################################
# Misc generators
########################################################################################################################

# Doxygen documentation
option(SIGHT_BUILD_DOC "Build the doxygen documentation" OFF)
if(SIGHT_BUILD_DOC)
    option(SIGHT_BUILD_DOCSET "Build a Dash/Zeal/XCode docset" OFF)
    include(${FWCMAKE_RESOURCE_PATH}doxygen/doxygen_generator.cmake)
    doxygengenerator(${PROJECT_LIST})
    if(SIGHT_BUILD_DOCSET)
        docsetgenerator(${PROJECT_LIST})
    endif()
else()
    unset(SIGHT_BUILD_DOCSET CACHE)
endif()
sight_create_package_targets("${COMPONENTS}" "")
