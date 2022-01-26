# This script is used in two different ways:
# On Linux, it is directly included in the main CMakeLists.txt
# On Windows, it is invoked from the command with cmake -P and optionnaly with arguments: -DSIGHT_DEPS_ROOT_DIRECTORY
# and -DGET_ARCHIVE_FOLDER

cmake_minimum_required(VERSION 3.18)

# Download and install package, if needed
function(
    download_deps
    root_directory
    package
    archive
    archive_hash
    url
    basename
)
    # Retrieve the package directory
    get_filename_component(package_directory "${root_directory}/${package}" REALPATH)

    if(NOT EXISTS "${package_directory}")

        # Download the sight deps archive
        message(STATUS "Downloading ${archive}...")
        file(DOWNLOAD "${url}" "${root_directory}/${archive}" SHOW_PROGRESS EXPECTED_HASH SHA256=${archive_hash})

        # Extract it
        # cmake-lint: disable=E1126
        message(STATUS "Extracting ${archive}...")
        file(ARCHIVE_EXTRACT INPUT "${root_directory}/${archive}" DESTINATION "${root_directory}" VERBOSE)

        # Cleanup
        file(REMOVE "${root_directory}/${archive}")

        if(NOT "${basename}" STREQUAL "")
            # Rename the extracted directory to ${package} to include commit hash, os version, build type.
            file(RENAME "${root_directory}/${basename}" "${root_directory}/${package}")
        endif()
    else()
        message(STATUS "Download of ${archive} skipped, already present.")
    endif()

    # Use ${package_directory} as external libraries
    set(SIGHT_EXTERNAL_LIBRARIES ${package_directory} PARENT_SCOPE)
endfunction()

if(WIN32)
    if(NOT SIGHT_DEPS_BASENAME)
        set(SIGHT_DEPS_BASENAME "sight-vcpkg")
    endif()

    if(NOT SIGHT_DEPS_PACKAGE)
        set(SIGHT_DEPS_PACKAGE "${SIGHT_DEPS_BASENAME}-8b0de93c")
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE)
        set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.7z")
    endif()

    if(NOT SIGHT_DEPS_PUBLIC_URL)
        set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/DW3Ru7zUQGV4d1h/download")
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE_HASH)
        set(SIGHT_DEPS_ARCHIVE_HASH "83de55319107f76c38da6cb4b625fcfcb5e4851eacdbbdd40168fa4a37831708")
    endif()

    # By default, we avoid to download binary packages inside the build tree on windows
    set(OUTPUT ".")
else()
    # Find the Linux version
    execute_process(COMMAND lsb_release -i -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_NAME)
    execute_process(COMMAND lsb_release -r -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_VERSION)
    execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)

    if(NOT SIGHT_DEPS_PACKAGE)
        set(SIGHT_DEPS_PACKAGE
            "sight-deps-21-0-0-2-geb877f3-${LINUX_NAME}-${LINUX_VERSION}-${CMAKE_BUILD_TYPE}-${ARCHITECTURE}"
        )
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE)
        set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.tar.zst")
    endif()

    if(NOT SIGHT_DEPS_PUBLIC_URL OR NOT SIGHT_DEPS_ARCHIVE_HASH)
        if("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "21.04")
            if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/0R2oAv8jVWdGMtC/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "d5cfc332414e9e256820b77a8e37612afa571bf1a6888fabdf97a061f2646724")
            elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/1TMxbBmAB8vMwzA/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "d72f481ea83c9ba1c2a90bad0999b76230ec107834a632beb634040fd4f13f3f")
            else()
                # RelWithDebInfo (DEFAULT)
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/pXQArciqbYHSLvW/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "2f02e359831ab25d990d5008404372ad84a6707cfa8ab283ef3c07bc834246ea")
            endif()
        elseif("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "21.10")
            if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/Lg6oV2UjosyvKxs/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "95f0cb26c4fbffa6775934d9fa41006b6caa7062d219510ae1a78c30e385f807")
            elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/4H30wGNgMet06L0/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "c87282fb3358cdedf57664e12fa7a854fab0c4d3b7e5f0eeb7141206279370ae")
            else()
                # RelWithDebInfo (DEFAULT)
                set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/mV6y1lHk5AYPDLX/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "b0ef945f7a6a7acf4ad7bd2bd19daebdf161bdff42b4036360f1c44a591d956f")
            endif()
        else()
            message(FATAL_ERROR "Unsupported linux distribution: ${LINUX_NAME} ${LINUX_VERSION}")
        endif()
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

download_deps(
    ${SIGHT_DEPS_ROOT_DIRECTORY} ${SIGHT_DEPS_PACKAGE} ${SIGHT_DEPS_ARCHIVE} ${SIGHT_DEPS_ARCHIVE_HASH}
    ${SIGHT_DEPS_PUBLIC_URL} "${SIGHT_DEPS_BASENAME}"
)

message("Using ${SIGHT_EXTERNAL_LIBRARIES} for Sight external libraries")

unset(OUTPUT)
unset(SIGHT_DEPS_PUBLIC_URL)
unset(SIGHT_DEPS_ARCHIVE_HASH)
unset(SIGHT_DEPS_ARCHIVE)
unset(SIGHT_DEPS_PACKAGE)
unset(SIGHT_DEPS_BASENAME)
