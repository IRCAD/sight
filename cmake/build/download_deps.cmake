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
        set(SIGHT_DEPS_PACKAGE "${SIGHT_DEPS_BASENAME}-0c1939bb")
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE)
        set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.7z")
    endif()

    if(NOT SIGHT_DEPS_PUBLIC_URL)
        set(SIGHT_DEPS_PUBLIC_URL "https://cloud.ircad.fr/s/yeRcWygdf3Wt3bQ/download")
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE_HASH)
        set(SIGHT_DEPS_ARCHIVE_HASH "c92ed0e215cdd6563c20ef81fac852c624c72806f9f0d8177c4bbf6d2f54e86c")
    endif()

    # By default, we avoid to download binary packages inside the build tree on windows
    set(OUTPUT ".")
else()
    # Find the Linux version
    execute_process(COMMAND lsb_release -i -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_NAME)
    execute_process(COMMAND lsb_release -r -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_VERSION)
    execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)

    if(NOT SIGHT_DEPS_PACKAGE)
        if("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "22.04")
            set(SIGHT_DEPS_PACKAGE
                "sight-deps-22-1-0.beta-${LINUX_NAME}-${LINUX_VERSION}-${CMAKE_BUILD_TYPE}-${ARCHITECTURE}"
            )
        endif()
    endif()

    if(NOT SIGHT_DEPS_ARCHIVE)
        set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.tar.zst")
    endif()

    if(NOT SIGHT_DEPS_PUBLIC_URL OR NOT SIGHT_DEPS_ARCHIVE_HASH)
        if("${LINUX_NAME}" STREQUAL "Ubuntu" AND "${LINUX_VERSION}" STREQUAL "22.04")
            if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                set(SIGHT_DEPS_PUBLIC_URL "https://cloud.ircad.fr/s/CAEHNSbfApdBnbE/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "b65ae6d5d0e9dcc01fd3e078afc6b76789c63ed773c64e5ad756dae518ce92f6")
            elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
                set(SIGHT_DEPS_PUBLIC_URL "https://cloud.ircad.fr/s/7ryqBGSp9ttFAD3/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "3d56732d050bb698aee40144b00a04c517a37a637bb2a124434bd1a1136a0484")
            else()
                # RelWithDebInfo (DEFAULT)
                set(SIGHT_DEPS_PUBLIC_URL "https://cloud.ircad.fr/s/rJ6DFqyWYXRbzi9/download")
                set(SIGHT_DEPS_ARCHIVE_HASH "90c50f7a57aee2ffa7bdf101979753224a3fc4e1d56bf430ef8b98afe1d2e815")
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
