cmake_minimum_required(VERSION 3.18)

# Packages name and version
set(SIGHT_DEPS_PACKAGE "sight-vcpkg")
set(SIGHT_DEPS_PACKAGE_ARCHIVE "${SIGHT_DEPS_PACKAGE}.zip")
set(SIGHT_DEPS_PACKAGE_URL "https://conan.ircad.fr/artifactory/data/sight-deps/${SIGHT_DEPS_PACKAGE_ARCHIVE}")

if(NOT OUTPUT)
    set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/..")
endif()

# Set sight deps root directory (where the sight deps packages will be installed ) 
set(SIGHT_DEPS_ROOT_DIRECTORY "${OUTPUT}" CACHE PATH "Sight deps root directory")
mark_as_advanced(SIGHT_DEPS_ROOT_DIRECTORY)

# Todo
#set(SIGHT_DEPS_PACKAGE_HASH "6357c36c5e35327a2610bf3a02119a3b1bfa365e2a78ca705118e9b7e99bcd1b")

# Download and install package, if needed
get_filename_component(SIGHT_DEPS_PACKAGE_DIRECTORY "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}" REALPATH)
    
if(NOT EXISTS ${SIGHT_DEPS_PACKAGE_DIRECTORY})
    # Download the sight deps archive
    file(
        DOWNLOAD "${SIGHT_DEPS_PACKAGE_URL}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}"
        SHOW_PROGRESS
        #EXPECTED_HASH SHA256=${SIGHT_DEPS_PACKAGE_HASH}
    )

    # Extract it
    file(
    ARCHIVE_EXTRACT 
    INPUT "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}"
    DESTINATION "${SIGHT_DEPS_ROOT_DIRECTORY}"
    )

    file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE_ARCHIVE}")
endif()

unset(SIGHT_DEPS_PACKAGE_DIRECTORY)
unset(SIGHT_DEPS_PACKAGE_HASH)
unset(SIGHT_DEPS_PACKAGE_URL)
unset(SIGHT_DEPS_PACKAGE_ARCHIVE)
unset(SIGHT_DEPS_PACKAGE)
