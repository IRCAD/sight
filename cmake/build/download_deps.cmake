# This script is used in two different ways:
# On Linux, it is directly included in the main CMakeLists.txt
# On Windows, it is invoked from the command with cmake -P and optionnaly with arguments: -DOUTPUT and -DGET_ARCHIVE_FOLDER

cmake_minimum_required(VERSION 3.18)

if(WIN32)
    set(SIGHT_DEPS_BASENAME "sight-vcpkg")
    set(SIGHT_DEPS_COMMIT "b1d40499")
    set(SIGHT_DEPS_EXTENSION ".zip")

    set(SIGHT_DEPS_PACKAGE_HASH "c4adcde58ca6d513d901d341efffc64123e706e1be41e97405eb9b365dd54adf")
                
    # By default, we avoid to download binary packages inside the build tree on windows
    set(OUTPUT ".")
else()
    set(SIGHT_DEPS_BASENAME "sight-deps-${CMAKE_BUILD_TYPE}-19.0.0-16")
    set(SIGHT_DEPS_COMMIT "g5b9e69b")
    set(SIGHT_DEPS_SUFFIX "-${CMAKE_HOST_SYSTEM_NAME}")
    set(SIGHT_DEPS_EXTENSION "-dev.tar.zst")
    
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
        
    # Command line arguments used to specify the output directory
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/.sight-deps")
endif()

# Packages name and version
set(SIGHT_DEPS_PACKAGE "${SIGHT_DEPS_BASENAME}-${SIGHT_DEPS_COMMIT}${SIGHT_DEPS_SUFFIX}")
set(SIGHT_DEPS_PACKAGE_ARCHIVE "${SIGHT_DEPS_PACKAGE}${SIGHT_DEPS_EXTENSION}")
set(SIGHT_DEPS_PACKAGE_URL "https://conan.ircad.fr/artifactory/data/sight-deps/${SIGHT_DEPS_PACKAGE_ARCHIVE}")

# Command line argument used in the CI/CD to get the name of the downloaded package
if(GET_ARCHIVE_FOLDER)
    message("${SIGHT_DEPS_PACKAGE}")
    return()
endif()

# Set sight deps root directory (where the sight deps packages will be installed ) 
set(SIGHT_DEPS_ROOT_DIRECTORY "${OUTPUT}" CACHE PATH "Sight deps root directory")
mark_as_advanced(SIGHT_DEPS_ROOT_DIRECTORY)

# Download and install package, if needed
get_filename_component(SIGHT_DEPS_PACKAGE_DIRECTORY "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}" REALPATH)
    
if(NOT EXISTS "${SIGHT_DEPS_PACKAGE_DIRECTORY}")
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
    if(WIN32)
        # Rename the extracted folder to respect the commit hash
        file(RENAME "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_BASENAME}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}")
    endif()

    file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}")
else()
    message(STATUS "Download of ${SIGHT_DEPS_PACKAGE_ARCHIVE} skipped, already present.")
endif()

# Use ${SIGHT_DEPS_PACKAGE_DIRECTORY} as external libraries
set(SIGHT_EXTERNAL_LIBRARIES ${SIGHT_DEPS_PACKAGE_DIRECTORY})
message("Using ${SIGHT_EXTERNAL_LIBRARIES} for Sight external libraries")

unset(SIGHT_DEPS_PACKAGE_DIRECTORY)
unset(SIGHT_DEPS_PACKAGE_HASH)
unset(SIGHT_DEPS_PACKAGE_URL)
unset(SIGHT_DEPS_PACKAGE_ARCHIVE)
unset(SIGHT_DEPS_PACKAGE)
