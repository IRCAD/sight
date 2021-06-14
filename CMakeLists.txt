cmake_minimum_required (VERSION 3.18)

# Use new policy for 'install_name' and RPATH on macOS (use `cmake --help-policy CMP0068` for details)
cmake_policy(SET CMP0068 NEW)
# Use new policy for `FindOpenGL` to prefer GLVND by default when available on linux (use `cmake --help-policy CMP0072` for details).
cmake_policy(SET CMP0072 NEW)
# Use new policy for 'Honor visibility properties for all target types.' (use `cmake --help-policy CMP0063` for details)
cmake_policy(SET CMP0063 NEW)
# Adds support for the new IN_LIST operator.
cmake_policy(SET CMP0057 NEW)
# Use new policy to use CMAKE_CXX_STANDARD in try_compile() macro
cmake_policy(SET CMP0067 NEW)
# Use old policy for Documentation to add cache variables and find VTK documentation dependent packages. (Needed for doxygen..)
cmake_policy(SET CMP0106 OLD)

# On Windows, if the user doesn't specify a value,
# 'CMAKE_BUILD_TYPE' is automatically initialized to 'Debug' after 'project()'.
# So we need to check this variable at this point.
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug, Release, RelWithDebInfo")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}' (required Debug, Release, RelWithDebInfo)")
endif()

project(sight 
        VERSION 21.0.0
        DESCRIPTION   "Surgical Image Guidance and Healthcare Toolkit"
        HOMEPAGE_URL  "https://git.ircad.fr/sight/sight"
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
option(SIGHT_VERBOSE_PCH  "Display debug messages to help debugging PCH" OFF)
mark_as_advanced(SIGHT_ENABLE_PCH)
mark_as_advanced(SIGHT_VERBOSE_PCH )

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/macros.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/linux/modules)

# We find VTK to know its version so we can enable PCL or not
find_package(VTK CONFIG QUIET REQUIRED)

########################################################################################################################
# User options
########################################################################################################################

# Tests build / run options
set(SIGHT_BUILD_TESTS ON CACHE BOOL "Configures projects associated tests (<project>Test projects)")
set(SIGHT_TESTS_XML_OUTPUT OFF CACHE BOOL "Tests will generate an xml output, suitable for CI integration")
mark_as_advanced(SIGHT_TESTS_XML_OUTPUT)
set(SIGHT_TESTS_FILTER "" CACHE STRING "Allows to only build/run tests whose path contains the filter string.")
mark_as_advanced(SIGHT_TESTS_FILTER)

# QML_IMPORT_PATH allows qtCreator to find the qml modules created in our modules
set(QML_IMPORT_PATH "" CACHE STRING "Path of the Qml modules." FORCE)
mark_as_advanced(QML_IMPORT_PATH)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/PrecompiledHeader.cmake)

if(MSVC)
    if(NOT DEFINED CMAKE_PCH_COMPILER_TARGETS)
        # this will be executed in just before makefile generation
        variable_watch(CMAKE_BACKWARDS_COMPATIBILITY pch_msvc_hook)
    endif()
endif()

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "List of supported configurations." FORCE)
endif()

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER automoc)

# Make openvslam optional
option(SIGHT_ENABLE_OPENVSLAM "Enable OpenVSLAM" OFF)

# Make Realsense optional ()
option(SIGHT_ENABLE_REALSENSE "Enable RealSense" OFF)

########################################################################################################################
# Warn user of install dir isn't empty
########################################################################################################################

file(GLOB_RECURSE INSTALL_DIR_CONTENT ${CMAKE_INSTALL_PREFIX}/*)
list(LENGTH INSTALL_DIR_CONTENT CONTENT)
if(NOT CONTENT EQUAL 0)
    # DIR isn't empty, warn user.
    message(WARNING "CMAKE_INSTALL_PREFIX (${CMAKE_INSTALL_PREFIX}) isn't empty, please select another folder or clean it before running install command.")
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
set_property(GLOBAL PROPERTY SIGHT_COMPONENTS "")

# Define the path 'FW_SIGHT_EXTERNAL_LIBRARIES_DIR' used to find external libraries required by our applications
setExternalLibrariesDir()
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
add_subdirectory(tutorials)
add_subdirectory(examples)


########################################################################################################################
# Export and install targets
########################################################################################################################

# the SIGHT_COMPONENTS global property contains all components, but unsorted. We can't use them as is.
get_property(SIGHT_COMPONENTS GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS)

# Use the ordered list of components
order_sight_components("${SIGHT_COMPONENTS}" SIGHT_ORDERED_COMPONENTS)
message("-- Sight Components: ${SIGHT_ORDERED_COMPONENTS}")
set(SIGHT_COMPONENTS "${SIGHT_ORDERED_COMPONENTS}")

configure_file(${CMAKE_SOURCE_DIR}/cmake/build/sightConfig.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/sightConfig.cmake @ONLY)

# Create the sightConfigVersion file
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/cmake/sightConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/linux/safe-xvfb-run 
    ${CMAKE_CURRENT_BINARY_DIR}/bin/safe-xvfb-run
    COPYONLY
)

# Install the sightConfig.cmake and sightConfigVersion.cmake
install(
  FILES
    "${CMAKE_BINARY_DIR}/cmake/sightConfig.cmake"
    "${CMAKE_BINARY_DIR}/cmake/sightConfigVersion.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/macros.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/linux/modules/FindFilesystem.cmake"
  DESTINATION
    ${FWCONFIG_PACKAGE_LOCATION}
  COMPONENT
    dev
)

# install CppUnitConfig.cmake in a modules folder.
install( FILES "${CMAKE_SOURCE_DIR}/cmake/modules/CppUnitConfig.cmake"
    DESTINATION
      ${FWCONFIG_PACKAGE_LOCATION}/modules
    COMPONENT
      dev
  )

# Install some files needed for the build
install(
  FILES
    "${CMAKE_SOURCE_DIR}/cmake/build/configure_file.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/flags.cmake"
    "${FWCMAKE_RESOURCE_PATH}/build/cppunit_main.cpp"
    "${CMAKE_SOURCE_DIR}/cmake/build/config.hpp.in"
    "${CMAKE_SOURCE_DIR}/cmake/build/plugin_config.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/plugin_config_command.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/profile_config.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/build/profile.xml.in"
    "${CMAKE_SOURCE_DIR}/cmake/build/registerServices.cpp.in"
  DESTINATION
    ${FWCONFIG_PACKAGE_LOCATION}/build
  COMPONENT
    dev
)

# Install some files needed for the install
install(
  FILES
    "${CMAKE_SOURCE_DIR}/cmake/install/generic_install.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/install/get_git_rev.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/install/helper.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/install/install_imported.cmake.in"
    "${CMAKE_SOURCE_DIR}/cmake/install/pre_package.cmake"
  DESTINATION
    ${FWCONFIG_PACKAGE_LOCATION}/install
  COMPONENT
    dev
)

if(WIN32)
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/build/windows/template_exe.bat.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/build/windows
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/package.cmake"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/windows
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/install_plugins.cmake"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/template.bat.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/setpath.bat.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/windows_fixup.cmake.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/windows
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/NSIS.InstallOptions.ini.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/NSIS.template.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/banner_nsis.bmp"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/dialog_nsis.bmp"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/app.ico"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/license.rtf"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/rc/
    )
elseif(UNIX)
    install(
      FILES
      "${CMAKE_SOURCE_DIR}/cmake/build/linux/template.sh.in"
      "${CMAKE_SOURCE_DIR}/cmake/build/linux/template_exe.sh.in"
      "${CMAKE_SOURCE_DIR}/cmake/build/linux/template_test.sh.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/build/linux
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/package.cmake"
        
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/linux
      COMPONENT
        dev
    )

    install(
      PROGRAMS
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/safe-xvfb-run"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/linux
      COMPONENT
        dev
    )

    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/template.sh.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/template_exe.sh.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/linux_fixup.cmake.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/linux
      COMPONENT
        dev
    )
endif()

if(MSVC)
  add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/install/windows)
endif()

########################################################################################################################
# Misc generators
########################################################################################################################

# Doxygen documentation
option(SIGHT_BUILD_DOC "Build the doxygen documentation" OFF)
if(SIGHT_BUILD_DOC)
    option(SIGHT_BUILD_DOCSET "Build a Dash/Zeal/XCode docset" OFF)
    include(${FWCMAKE_RESOURCE_PATH}doxygen/doxygen_generator.cmake)
    doxygenGenerator(${PROJECT_LIST})
    if(SIGHT_BUILD_DOCSET)
        docsetGenerator(${PROJECT_LIST})
    endif()
else()
    unset(SIGHT_BUILD_DOCSET CACHE)
endif()


sight_create_package_targets("${SIGHT_COMPONENTS}" "")
