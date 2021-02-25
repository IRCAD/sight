cmake_minimum_required (VERSION 3.13)

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

# On Windows, if the user doesn't specify a value,
# 'CMAKE_BUILD_TYPE' is automatically initialized to 'Debug' after 'project()'.
# So we need to check this variable at this point.
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug, Release, RelWithDebInfo")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}' (required Debug, Release, RelWithDebInfo)")
endif()

project (sight)

set(SIGHT_REPOSITORY "sight")
set(SIGHT_VERSION "21.0.0")
set(SIGHT_API_VERSION "21.0")

if(APPLE)
    message(FATAL_ERROR "macOS is not supported.")
endif()

enable_testing()

include(CheckVariableExists)
include(CMakeParseArguments)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/macros.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/linux/modules)

########################################################################################################################
# User options
########################################################################################################################

# Tests build / run options
set(BUILD_TESTS ON CACHE BOOL "Configures projects associated tests (<project>Test projects)")
set(TESTS_XML_OUTPUT OFF CACHE BOOL "Tests will generate an xml output, suitable for CI integration")
mark_as_advanced(TESTS_XML_OUTPUT)
set(TESTS_FILTER "" CACHE STRING "Allows to only build/run tests whose path contains the filter string.")
mark_as_advanced(TESTS_FILTER)

option(ENABLE_PCH "Use pre-compiled headers to speedup the compilation" ON)
option(VERBOSE_PCH "Display debug messages to help debugging PCH" OFF)
mark_as_advanced(ENABLE_PCH)
mark_as_advanced(VERBOSE_PCH)

# QML_IMPORT_PATH allows qtCreator to find the qml modules created in our modules
set(QML_IMPORT_PATH "" CACHE STRING "Path of the Qml modules." FORCE)
mark_as_advanced(QML_IMPORT_PATH)
set( ENABLE_QML_APPLICATION OFF CACHE BOOL "Temporary Variable to set Qml or not in applications to kept old services working" )

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

set(CREATE_SUBPROJECTS OFF CACHE BOOL "Create a project for each subproject")

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER automoc)

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

if(NOT VCPKG_TARGET_TRIPLET)
    list(APPEND CMAKE_PREFIX_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    
    list(APPEND CMAKE_FIND_ROOT_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    if(UNIX)
        list(APPEND CMAKE_PREFIX_PATH /usr/share/OGRE/cmake/modules)
    endif()

    # Parsing of 'fw-*.cmake' scripts. This should go away at some point if we use CMake properly
    file(GLOB LIB_CONFIGS ${FWCMAKE_RESOURCE_PATH}/modules/fw-*.cmake )
    foreach(LIB_CONFIG ${LIB_CONFIGS})
        message(STATUS "include: ${LIB_CONFIG}")
        include("${LIB_CONFIG}")
    endforeach()

    set(EXTERNAL_LIBRARIES CACHE PATH "External libraries location")
    mark_as_advanced(EXTERNAL_LIBRARIES)

    # Use directly EXTERNAL_LIBRARIES if set, otherwise, download and install sight-deps package
    if(NOT EXTERNAL_LIBRARIES)
        # Set sight deps root directory (where the sight deps packages will be installed ) 
        set(SIGHT_DEPS_ROOT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.sight-deps" CACHE PATH "Sight deps root directory")
        mark_as_advanced(SIGHT_DEPS_ROOT_DIRECTORY)

        # Packages name and version
        set(SIGHT_DEPS_PACKAGE "sight-deps-${CMAKE_BUILD_TYPE}-19.0.0-16-g5b9e69b-${CMAKE_HOST_SYSTEM_NAME}")
        set(SIGHT_DEPS_PACKAGE_ARCHIVE "${SIGHT_DEPS_PACKAGE}-dev.tar.zst")
        set(SIGHT_DEPS_PACKAGE_URL "https://conan.ircad.fr/artifactory/data/sight-deps/${SIGHT_DEPS_PACKAGE_ARCHIVE}")

        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_DEPS_PACKAGE_HASH "6357c36c5e35327a2610bf3a02119a3b1bfa365e2a78ca705118e9b7e99bcd1b")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(SIGHT_DEPS_PACKAGE_HASH "e5b90189665145cb82d1a4404f6b09b7f8e19cfa241040f90710fcbd7534aac7")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
            set(SIGHT_DEPS_PACKAGE_HASH "bb1464671ed43672f2698696b6fd80645442f2af69455638b54b02f6f351c5dd")
        else()
            # Falback for other build type
            set(SIGHT_DEPS_PACKAGE_HASH "bb1464671ed43672f2698696b6fd80645442f2af69455638b54b02f6f351c5dd")
        endif()

        # Download and install package, if needed
        get_filename_component(SIGHT_DEPS_PACKAGE_DIRECTORY "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}" REALPATH)
            
        if(NOT EXISTS ${SIGHT_DEPS_PACKAGE_DIRECTORY})
            # Download the sight deps archive
            file(
                DOWNLOAD "${SIGHT_DEPS_PACKAGE_URL}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}"
                SHOW_PROGRESS
                EXPECTED_HASH SHA256=${SIGHT_DEPS_PACKAGE_HASH}
            )

            # Extract it
            file(
            ARCHIVE_EXTRACT 
            INPUT "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}"
            DESTINATION "${SIGHT_DEPS_ROOT_DIRECTORY}"
            VERBOSE
            )

            file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}")
        endif()

        # Use ${SIGHT_DEPS_PACKAGE_DIRECTORY} as external libraries
        set(EXTERNAL_LIBRARIES ${SIGHT_DEPS_PACKAGE_DIRECTORY})
        message("Using ${EXTERNAL_LIBRARIES} for Sight external libraries")

        unset(SIGHT_DEPS_PACKAGE_DIRECTORY)
        unset(SIGHT_DEPS_PACKAGE_HASH)
        unset(SIGHT_DEPS_PACKAGE_URL)
        unset(SIGHT_DEPS_PACKAGE_ARCHIVE)
        unset(SIGHT_DEPS_PACKAGE)
    endif()

    if(EXTERNAL_LIBRARIES)
        get_filename_component(ABSOLUTE_EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} REALPATH)
        set(EXTERNAL_LIBRARIES ${ABSOLUTE_EXTERNAL_LIBRARIES})
        unset(ABSOLUTE_EXTERNAL_LIBRARIES)

        list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARIES})
        list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_LIBRARIES}/lib/cmake/)
        list(APPEND CMAKE_FIND_ROOT_PATH ${EXTERNAL_LIBRARIES})
    endif()
endif()

if(BUILD_TESTS)
    find_package(CppUnit QUIET REQUIRED)
endif(BUILD_TESTS)

########################################################################################################################
# Default paths settings for libraries, modules and resources
########################################################################################################################

set(FW_INSTALL_PATH_SUFFIX "${SIGHT_REPOSITORY}")
set(SIGHT_MODULE_RC_PREFIX "${CMAKE_INSTALL_DATADIR}/${FW_INSTALL_PATH_SUFFIX}")
if(WIN32)
    set(SIGHT_MODULE_LIB_PREFIX "${CMAKE_INSTALL_BINDIR}")
else()
    set(SIGHT_MODULE_LIB_PREFIX "${CMAKE_INSTALL_LIBDIR}")
endif()

# Fixed path for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR} )
endforeach()

set(FWCONFIG_PACKAGE_LOCATION lib/cmake/sight)
set_property(GLOBAL PROPERTY SIGHT_COMPONENTS "")

add_subdirectory(libs)
add_subdirectory(modules)
add_subdirectory(configs)
add_subdirectory(activities) 
add_subdirectory(utils)
add_subdirectory(apps)
add_subdirectory(tutorials)
add_subdirectory(examples)

# Define the path 'FW_EXTERNAL_LIBRARIES_DIR' used to find external libraries required by our applications
setExternalLibrariesDir()

########################################################################################################################
# Export and install targets
########################################################################################################################

# the SIGHT_COMPONENTS global property contains all components, but unsorted. We can't use them as is.
get_property(SIGHT_COMPONENTS GLOBAL PROPERTY SIGHT_COMPONENTS)

# For now, use a hardcoded list, we may compute this later from SIGHT_COMPONENTS 
set(SIGHT_COMPONENTS "core;pchCore;data;pchData;activity;service;pchServices;pchServicesOmp;atoms;geometry_eigen;geometry_data;geometry_glm;geometry_vision;io_base;io_itk;filter_vision;filter_image;filter_dicom;ui_history;ui_base;ui_qml;ui_qt;ui_dicom;io_zip;io_dicom;io_atoms;io_igtl;io_http;io_patch;io_vtk;io_opencv;io_dimse;navigation_openvslam;viz_base;viz_qt3d;viz_scene2d;viz_scene3d;module_memory;module_appXml;module_sync;module_activity;module_debug;module_service;module_data;module_filter_image;module_filter_point;module_filter_vision;module_filter_mesh;module_geometry_vision;module_geometry_base;module_geometry_generator;module_io_video;module_io_dimse;module_io_itk;module_io_vtk;module_io_vision;module_io_pcl;module_io_atoms;module_io_igtl;module_io_dicomweb;module_io_document;module_io_matrix;module_io_dicom;module_io_realsense;module_navigation_calibration;module_navigation_openvslam;module_navigation_optics;module_ui_debug;module_ui_viz;module_ui_dicom;module_ui_console;module_ui_base;module_ui_qml;module_ui_qt;module_ui_history;module_viz_scene3dQt;module_viz_scene2d;module_viz_scene3d;module_viz_sample;module_viz_qt3d;module_ui_flaticons;config_viz_scene3d;activity_navigation_calibration")

configure_file(${CMAKE_SOURCE_DIR}/cmake/build/sightConfig.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/sightConfig.cmake @ONLY)

# Create the sightConfigVersion file
set(SIGHT_VERSION ${GIT_TAG})

write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/cmake/sightConfigVersion.cmake"
  VERSION ${SIGHT_VERSION}
  COMPATIBILITY AnyNewerVersion
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

# Install some files needed for the build
install(
  FILES
    "${CMAKE_SOURCE_DIR}/cmake/build/configure_file.cmake"
    "${FWCMAKE_RESOURCE_PATH}/cppunit/cppunit_main.cpp"
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
    "${CMAKE_SOURCE_DIR}/cmake/install/helper.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/install/get_git_rev.cmake"
  DESTINATION
    ${FWCONFIG_PACKAGE_LOCATION}/install
  COMPONENT
    dev
)

if(WIN32)
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/win_install.cmake"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install
      COMPONENT
        dev
    )
    install(
      FILES
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
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/banner_nsis.bmp"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/dialog_nsis.bmp"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/app.ico"
        "${CMAKE_SOURCE_DIR}/cmake/install/windows/NSIS/rc/license.rtf"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/rc/
      COMPONENT
        dev
    )
elseif(UNIX)
    install(
      FILES
      "${CMAKE_SOURCE_DIR}/cmake/build/linux/template.sh.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/build/linux
      COMPONENT
        dev
    )
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/linux_install.cmake"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install
      COMPONENT
        dev
    )

    install(
      FILES
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/template.sh.in"
        "${CMAKE_SOURCE_DIR}/cmake/install/linux/linux_fixup.cmake.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install/linux
      COMPONENT
        dev
    )
endif()


########################################################################################################################
# Misc generators
########################################################################################################################

# Doxygen documentation
option(BUILD_DOCUMENTATION "Build the doxygen documentation" OFF)
if(BUILD_DOCUMENTATION)
    option(BUILD_DOCSET "Build a Dash/Zeal/XCode docset" OFF)
    include(${FWCMAKE_RESOURCE_PATH}doxygen/doxygen_generator.cmake)
    doxygenGenerator(${PROJECT_LIST})
    if(BUILD_DOCSET)
        docsetGenerator(${PROJECT_LIST})
    endif()
else()
    unset(BUILD_DOCSET CACHE)
endif()

# VS Code project
option(GENERATE_VSCODE_WS "Generate VS Code workspace" OFF)
if(GENERATE_VSCODE_WS)
    include(${FWCMAKE_RESOURCE_PATH}vscode/vscode_generator.cmake)
    vscodeGenerator(${PROJECT_LIST})
endif()

