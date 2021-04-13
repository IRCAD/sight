cmake_minimum_required(VERSION 3.18)

# Packages name and version
set(SIGHT_DEPS_BASENAME "sight-vcpkg")
set(SIGHT_DEPS_COMMIT "b1d40499")
set(SIGHT_DEPS_PACKAGE "${SIGHT_DEPS_BASENAME}-${SIGHT_DEPS_COMMIT}")
set(SIGHT_DEPS_PACKAGE_ARCHIVE "${SIGHT_DEPS_PACKAGE}.zip")
set(SIGHT_DEPS_PACKAGE_URL "https://conan.ircad.fr/artifactory/data/sight-deps/${SIGHT_DEPS_PACKAGE_ARCHIVE}")

if(GET_ARCHIVE_FOLDER)
    message("${SIGHT_DEPS_PACKAGE}")
    return()
endif()

if(NOT OUTPUT)
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/..")
endif()

# Set sight deps root directory (where the sight deps packages will be installed ) 
set(SIGHT_DEPS_ROOT_DIRECTORY "${OUTPUT}" CACHE PATH "Sight deps root directory")
mark_as_advanced(SIGHT_DEPS_ROOT_DIRECTORY)

# Todo
set(SIGHT_DEPS_PACKAGE_HASH "c4adcde58ca6d513d901d341efffc64123e706e1be41e97405eb9b365dd54adf")

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
    )

    # Rename the extracted folder to respect the commit hash
    file(RENAME "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_BASENAME}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}")

    file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}")
else()
    message(STATUS "Download of ${SIGHT_DEPS_PACKAGE_ARCHIVE} skipped, already present.")
endif()

unset(SIGHT_DEPS_PACKAGE_DIRECTORY)
unset(SIGHT_DEPS_PACKAGE_HASH)
unset(SIGHT_DEPS_PACKAGE_URL)
unset(SIGHT_DEPS_PACKAGE_ARCHIVE)
unset(SIGHT_DEPS_PACKAGE)
