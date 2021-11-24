# This script is used in two different ways:
# On Linux, it is directly included in the main CMakeLists.txt
# On Windows, it is invoked from the command with cmake -P and optionnaly with arguments: -DOUTPUT and
# -DGET_ARCHIVE_FOLDER

cmake_minimum_required(VERSION 3.18)

if(WIN32)
    set(SIGHT_DEPS_BASENAME "sight-vcpkg")
    set(SIGHT_DEPS_PACKAGE "${SIGHT_DEPS_BASENAME}-fcf22805")
    set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.zip")
    set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/cCHh9UtXttko6s9/download")
    set(SIGHT_DEPS_PACKAGE_HASH "d09d7a0f17ca3e5d52c420455da6137faf692d0693d84a5bbaeeeb9228edf8d1")

    # By default, we avoid to download binary packages inside the build tree on windows
    set(OUTPUT ".")
else()
    # Find the Linux version
    execute_process(COMMAND lsb_release -i -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_NAME)
    execute_process(COMMAND lsb_release -r -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_VERSION)
    execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)

    set(SIGHT_DEPS_PACKAGE
        "sight-deps-21-0-0-2-geb877f3-${LINUX_NAME}-${LINUX_VERSION}-${CMAKE_BUILD_TYPE}-${ARCHITECTURE}"
    )
    set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.tar.zst")

    if("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "21.04")
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/0R2oAv8jVWdGMtC/download")
            set(SIGHT_DEPS_PACKAGE_HASH "d5cfc332414e9e256820b77a8e37612afa571bf1a6888fabdf97a061f2646724")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/1TMxbBmAB8vMwzA/download")
            set(SIGHT_DEPS_PACKAGE_HASH "d72f481ea83c9ba1c2a90bad0999b76230ec107834a632beb634040fd4f13f3f")
        else()
            # RelWithDebInfo (DEFAULT)
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/pXQArciqbYHSLvW/download")
            set(SIGHT_DEPS_PACKAGE_HASH "2f02e359831ab25d990d5008404372ad84a6707cfa8ab283ef3c07bc834246ea")
        endif()
    elseif("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "21.10")
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/Lg6oV2UjosyvKxs/download")
            set(SIGHT_DEPS_PACKAGE_HASH "95f0cb26c4fbffa6775934d9fa41006b6caa7062d219510ae1a78c30e385f807")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/4H30wGNgMet06L0/download")
            set(SIGHT_DEPS_PACKAGE_HASH "c87282fb3358cdedf57664e12fa7a854fab0c4d3b7e5f0eeb7141206279370ae")
        else()
            # RelWithDebInfo (DEFAULT)
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/mV6y1lHk5AYPDLX/download")
            set(SIGHT_DEPS_PACKAGE_HASH "b0ef945f7a6a7acf4ad7bd2bd19daebdf161bdff42b4036360f1c44a591d956f")
        endif()
    else()
        message(FATAL_ERROR "Unsupported linux distribution: ${LINUX_NAME} ${LINUX_VERSION}")
    endif()

    # Command line arguments used to specify the output directory
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/.sight-deps")
endif()

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
    file(DOWNLOAD "${SIGHT_DEPS_PUBLIC_URL}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}" SHOW_PROGRESS
         EXPECTED_HASH SHA256=${SIGHT_DEPS_PACKAGE_HASH}
    )

    # Extract it
    # cmake-lint: disable=E1126
    file(ARCHIVE_EXTRACT INPUT "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}" DESTINATION
         "${SIGHT_DEPS_ROOT_DIRECTORY}" VERBOSE
    )

    # Cleanup
    file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}")

    if(SIGHT_DEPS_BASENAME)
        # Rename the extracted directory to ${SIGHT_DEPS_PACKAGE} to include commit hash, os version, build type.
        file(RENAME "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_BASENAME}"
             "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}"
        )
    endif()
else()
    message(STATUS "Download of ${SIGHT_DEPS_PACKAGE_ARCHIVE} skipped, already present.")
endif()

# Use ${SIGHT_DEPS_PACKAGE_DIRECTORY} as external libraries
set(SIGHT_EXTERNAL_LIBRARIES ${SIGHT_DEPS_PACKAGE_DIRECTORY})
message("Using ${SIGHT_EXTERNAL_LIBRARIES} for Sight external libraries")

unset(SIGHT_DEPS_PACKAGE_DIRECTORY)
unset(OUTPUT)
unset(SIGHT_DEPS_PUBLIC_URL)
unset(SIGHT_DEPS_PACKAGE_HASH)
unset(SIGHT_DEPS_ARCHIVE)
unset(SIGHT_DEPS_PACKAGE)
unset(SIGHT_DEPS_BASENAME)
