# This script is used in two different ways:
# On Linux, it is directly included in the main CMakeLists.txt
# On Windows, it is invoked from the command with cmake -P and optionnaly with arguments: -DOUTPUT and -DGET_ARCHIVE_FOLDER

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

    set(SIGHT_DEPS_PACKAGE "sight-deps-19-0-0-19-g59a1b24-${LINUX_NAME}-${LINUX_VERSION}-${CMAKE_BUILD_TYPE}-${ARCHITECTURE}")
    set(SIGHT_DEPS_ARCHIVE "${SIGHT_DEPS_PACKAGE}.tar.zst")    
    
    if("${LINUX_VERSION}" STREQUAL "20.10")
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/caCCGDZMZZMg7uG/download")
            set(SIGHT_DEPS_PACKAGE_HASH "55f30102eac33361b26808593c82d8c3727980850bb5bea4a23ffd9bf7020405")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/vW1N8KPoLKlLJ6C/download")
            set(SIGHT_DEPS_PACKAGE_HASH "794ff689f63178f9bbfc65894cd331c1a1a4307199b9b3429897453ceead20a4")
        else()
            # RelWithDebInfo (DEFAULT)
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/huQEEPbJ0Hxs733/download")
            set(SIGHT_DEPS_PACKAGE_HASH "998ffc99799b36c9cb266a19ab98fcc51da800a16544320e0bd5cceedddffffa")
        endif()        
    else()
        # 21.04 (DEFAULT)
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/yNQH9ecuO6voqp9/download")
            set(SIGHT_DEPS_PACKAGE_HASH "a19f8404ec1c01eec627e10ea6d1a3df6649552502a63d240c1696a325bd81fa")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/vpllWT5daEzxmxt/download")
            set(SIGHT_DEPS_PACKAGE_HASH "ae93af3c2189acc0bb65faf8b2dec5fd890b2544a2f882b4365a185e13c6010b")
        else()
            # RelWithDebInfo (DEFAULT)
            set(SIGHT_DEPS_PUBLIC_URL "https://owncloud.ircad.fr/index.php/s/Z8xrbiyVuT5OOIG/download")
            set(SIGHT_DEPS_PACKAGE_HASH "b41cb80af4941b94e1a96b4302442c602753218a2e94adff9aa9f5612697390a")
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

# Download and install package, if needed
get_filename_component(SIGHT_DEPS_PACKAGE_DIRECTORY "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}" REALPATH)
    
if(NOT EXISTS "${SIGHT_DEPS_PACKAGE_DIRECTORY}")

    # Download the sight deps archive
    file(
        DOWNLOAD "${SIGHT_DEPS_PUBLIC_URL}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}"
        SHOW_PROGRESS
        EXPECTED_HASH SHA256=${SIGHT_DEPS_PACKAGE_HASH}
    )

    # Extract it
    file(
        ARCHIVE_EXTRACT 
        INPUT "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}"
        DESTINATION "${SIGHT_DEPS_ROOT_DIRECTORY}"
        VERBOSE
    )

    # Cleanup
    file(REMOVE "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_ARCHIVE}")

    if(SIGHT_DEPS_BASENAME)
        # Rename the extracted directory to ${SIGHT_DEPS_PACKAGE}, so we can discredit commit hash, os version, build type.
        file(RENAME "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_BASENAME}" "${SIGHT_DEPS_ROOT_DIRECTORY}/${SIGHT_DEPS_PACKAGE}")
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
