cmake_minimum_required (VERSION 3.1)

project (sight)

enable_testing()

include(CheckVariableExists)
include(CMakeParseArguments)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/macros.cmake)

########################################################################################################################
# User options
########################################################################################################################

option(USE_CONAN "Use Conan package manager for dependencies (experimental)" OFF)
mark_as_advanced(USE_CONAN)

if(USE_CONAN)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/conan.cmake)
endif()

# Tests build / run options
set(BUILD_TESTS ON CACHE BOOL "Configures projects associated tests (<project>Test projects)")
set(TESTS_XML_OUTPUT OFF CACHE BOOL "Tests will generate an xml output, suitable for CI integration")
mark_as_advanced(TESTS_XML_OUTPUT)
set(TESTS_FILTER "" CACHE STRING "Allows to only build/run tests whose path contains the filter string.")
mark_as_advanced(TESTS_FILTER)

set(SET_API_VERSION ON)

option(ENABLE_PCH "Use pre-compiled headers to speedup the compilation" ON)
option(VERBOSE_PCH "Display debug messages to help debugging PCH" OFF)
mark_as_advanced(ENABLE_PCH)
mark_as_advanced(VERBOSE_PCH)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/PrecompiledHeader.cmake)

if(MSVC)
    if(NOT DEFINED CMAKE_PCH_COMPILER_TARGETS)
        # this will be executed in just before makefile generation
        variable_watch(CMAKE_BACKWARDS_COMPATIBILITY pch_msvc_hook)
    endif()
endif(MSVC)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(SEND_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} (required Debug, Release, RelWithDebInfo or MinSizeRel)")
endif()
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "List of supported configurations." FORCE)
endif()

set(CREATE_SUBPROJECTS OFF CACHE BOOL "Create a project for each subproject")

set(PROJECTS_TO_BUILD CACHE STRING
        "List of projects that will be configured for build.
        Leave empty to configure all projects"
    )

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER automoc)

set(SPYLOG_LEVEL "error" CACHE STRING "Log level")
set(SPYLOG_LEVEL_VALUES "fatal;error;warning;info;debug;trace" CACHE INTERNAL
    "List of possible values for log level")
set(SPYLOG_LEVEL_MAP_fatal 1)
set(SPYLOG_LEVEL_MAP_error 2)
set(SPYLOG_LEVEL_MAP_warning 3)
set(SPYLOG_LEVEL_MAP_info 4)
set(SPYLOG_LEVEL_MAP_debug 5)
set(SPYLOG_LEVEL_MAP_trace 6)
set_property(CACHE SPYLOG_LEVEL PROPERTY STRINGS ${SPYLOG_LEVEL_VALUES} )

########################################################################################################################
# Compile flags
########################################################################################################################
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC AND ENABLE_PCH)
    # Store debug information in the .obj file instead of a PDB.
    string(REGEX REPLACE "/Z[iI]" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "/Z[iI]" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    # Force set debug compile flags.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Debug flags" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "RelWithDebInfo flags" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")  #racy backward compatibility

# Set warning level and function export visibility on Unix
if(UNIX)
    if(APPLE)
        # actually the flag -fvisibility=hidden makes application crash on OSX
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
        set(CMAKE_MACOSX_RPATH ON)
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility=hidden")
    endif()
    # Color for ninja and Clang on Linux and OSX
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        #Color output for gcc > 4.9
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8.99")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
        endif()
    endif()
endif()

# Building with all your cores on Visual Studio IDE
if(MSVC_IDE)
    set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if(${WIN32} AND ${MSVC})
    if(${MSVC_VERSION} GREATER 1914)
        add_definitions(" -D_ENABLE_EXTENDED_ALIGNED_STORAGE")
    endif()
endif()

if(EXTERNAL_LIBRARIES)
    get_filename_component(ABSOLUTE_EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} REALPATH)
    set(EXTERNAL_LIBRARIES ${ABSOLUTE_EXTERNAL_LIBRARIES})
    unset(ABSOLUTE_EXTERNAL_LIBRARIES)

    list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_FIND_ROOT_PATH ${EXTERNAL_LIBRARIES})
    include(${EXTERNAL_LIBRARIES}/lib/cmake/BinPkgsConfig.cmake)
    include(${EXTERNAL_LIBRARIES}/lib/cmake/find_sse.cmake)

    # This serves two purposes:
    # - The CUDA bundles/libs in Sight projects will automatically find a working CUDA toolkit.
    # - Dependencies using CUDA such as OpenCV can find the toolkit they were compiled with, they might otherwise pick
    #   another one if you have several toolkits installed, which would cause an error.
    set(CUDA_TOOLKIT_ROOT_DIR ${BINPKGS_CUDA_TOOLKIT_ROOT_DIR} CACHE PATH
        "Version of the CUDA toolkit used for this build, if any. Read-only."
        FORCE)
endif()

if(BINPKGS_HAS_SSE)
    message(STATUS "SSE support detected in the dependencies, enabling SSE.")
    # Build with SSE support if possible
    fwFindSSE()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SSE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSE_FLAGS}")
    add_definitions("${SSE_DEFINITIONS}")
endif()

########################################################################################################################
# External libraries management
########################################################################################################################

if(NOT USE_CONAN)
    option(USE_SYSTEM_LIB "Use system libraries to compile sight" OFF)
endif()

option(BUILD_SDK "Build Sight as a SDK" OFF)

if(NOT USE_SYSTEM_LIB AND NOT USE_CONAN)
    set(EXTERNAL_LIBRARIES CACHE PATH "External libraries location")
endif()

set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

# Append our 'FindPackages.cmake' to CMAKE_MODULE_PATH and include BinPkgs config file
if(EXTERNAL_LIBRARIES)

    list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_FIND_ROOT_PATH ${EXTERNAL_LIBRARIES})

elseif(USE_SYSTEM_LIB)

    list(APPEND CMAKE_PREFIX_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    list(APPEND CMAKE_FIND_ROOT_PATH ${FWCMAKE_RESOURCE_PATH}/modules)

endif()

# Parsing of 'fw-*.cmake' scripts. This should go away at some point if we use CMake properly
if(EXTERNAL_LIBRARIES)

    file(GLOB LIB_CONFIGS ${EXTERNAL_LIBRARIES}/fw-*.cmake )
    foreach(LIB_CONFIG ${LIB_CONFIGS})
        message(STATUS "include: ${LIB_CONFIG}")
        include("${LIB_CONFIG}")
    endforeach()

elseif(USE_SYSTEM_LIB)

    file(GLOB LIB_CONFIGS ${FWCMAKE_RESOURCE_PATH}/modules/fw-*.cmake )
    foreach(LIB_CONFIG ${LIB_CONFIGS})
        message(STATUS "include: ${LIB_CONFIG}")
        include("${LIB_CONFIG}")
    endforeach()

endif()

########################################################################################################################
# Automatic detection of additional repositories using .sight files
########################################################################################################################
file(GLOB ADDITIONAL_REPOSITORIES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../*/.sight ${CMAKE_CURRENT_SOURCE_DIR}/../*/.fw4spl)

foreach(REPOSITORY ${ADDITIONAL_REPOSITORIES_FILES})
    file(READ ${REPOSITORY} REPO_RAW_NAME)
    string(STRIP ${REPO_RAW_NAME} REPO_RAW_NAME)
    string(TOUPPER ${REPO_RAW_NAME} REPO_NAME)

    if(NOT ${REPO_NAME} STREQUAL "SIGHT")

        get_filename_component(REPO_DIR ${REPOSITORY} DIRECTORY)
        get_filename_component(REPO_DIR ${REPO_DIR} ABSOLUTE)

        option(BUILD_${REPO_NAME} "Enable ${REPO_NAME} repository" ON)
        list(APPEND ADDITIONAL_REPOSITORIES_OPTION BUILD_${REPO_NAME})
        list(APPEND ADDITIONAL_REPOSITORIES_NAME ${REPO_RAW_NAME})
        list(APPEND ADDITIONAL_REPOSITORIES ${REPO_DIR})
    endif()
endforeach()

# parse additional CMakeLists
if(ADDITIONAL_REPOSITORIES)
    list(LENGTH ADDITIONAL_REPOSITORIES ADDITIONAL_REPOSITORIES_LEN)

    math(EXPR ADDITIONAL_REPOSITORIES_LEN ${ADDITIONAL_REPOSITORIES_LEN}-1)
    foreach(REPO_INDEX RANGE ${ADDITIONAL_REPOSITORIES_LEN})
        list(GET ADDITIONAL_REPOSITORIES ${REPO_INDEX} ADDITIONAL_REPO)
        list(GET ADDITIONAL_REPOSITORIES_OPTION ${REPO_INDEX} ADDITIONAL_REPO_OPTION)

        if(EXISTS ${ADDITIONAL_REPO}/CMakeLists.txt AND ${ADDITIONAL_REPO_OPTION})
            get_filename_component(DIR_NAME ${ADDITIONAL_REPO} NAME)
            add_subdirectory(${ADDITIONAL_REPO} ${DIR_NAME})
        endif()
    endforeach()
endif()

########################################################################################################################
# Default paths settings for libraries, bundles and resources
########################################################################################################################

if(WIN32)
    set(FW_INSTALL_PATH_SUFFIX )
    set(FWBUNDLE_LIB_PREFIX "${CMAKE_INSTALL_BINDIR}")
    set(FWBUNDLE_RC_PREFIX "${CMAKE_INSTALL_DATADIR}")
else()
    set(FW_INSTALL_PATH_SUFFIX "sight")
    set(FWBUNDLE_LIB_PREFIX "${CMAKE_INSTALL_LIBDIR}/${FW_INSTALL_PATH_SUFFIX}")
    set(FWBUNDLE_RC_PREFIX "${CMAKE_INSTALL_DATADIR}/${FW_INSTALL_PATH_SUFFIX}")
endif()

set(LIBRARY_OUTPUT_DIR lib/${FW_INSTALL_PATH_SUFFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIBRARY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIBRARY_OUTPUT_DIR})
# Fixed path for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${LIBRARY_OUTPUT_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${LIBRARY_OUTPUT_DIR} )
endforeach()

########################################################################################################################
# The Properties.cmake files are the entry point to scan for targets
# We really start the configure process here
########################################################################################################################
file(GLOB_RECURSE PROJECTS_PROPERTIES */Properties.cmake)

# Do the same in additional repositories
if(ADDITIONAL_REPOSITORIES)

    list(LENGTH ADDITIONAL_REPOSITORIES ADDITIONAL_REPOSITORIES_LEN)
    math(EXPR ADDITIONAL_REPOSITORIES_LEN ${ADDITIONAL_REPOSITORIES_LEN}-1)

    foreach(REPO_INDEX RANGE -${ADDITIONAL_REPOSITORIES_LEN} - 1)
        list(GET ADDITIONAL_REPOSITORIES ${REPO_INDEX} ADDITIONAL_REPO)
        list(GET ADDITIONAL_REPOSITORIES_OPTION ${REPO_INDEX} ADDITIONAL_REPO_OPTION)

        if(${ADDITIONAL_REPO_OPTION})
            list(GET ADDITIONAL_REPOSITORIES_NAME ${REPO_INDEX} ADDITIONAL_REPO_NAME)
            message(STATUS "Configuring additional repository " ${ADDITIONAL_REPO_NAME} " found in: " ${ADDITIONAL_REPO})
            file(GLOB_RECURSE ADDITIONAL_REPOSITORIES_PROPERTIES ${ADDITIONAL_REPO}/*/Properties.cmake)
            list(APPEND PROJECTS_PROPERTIES ${ADDITIONAL_REPOSITORIES_PROPERTIES})
            set(ADDITIONAL_REPOSITORIES_SOURCE_DIR "${ADDITIONAL_REPOSITORIES_SOURCE_DIR} ${ADDITIONAL_REPO}")
        endif()
    endforeach()
endif()

set(FWCONFIG_PACKAGE_LOCATION lib/cmake/sight)

unset(PROJECT_LIST)

# Parse all Properties.cmake files and populate a list of CMake variables for each projet
# to know its dependencies, requirements, version, pch usage, etc...
foreach(PROPERTIES_FILE ${PROJECTS_PROPERTIES})
    get_filename_component(PROJECT_DIR ${PROPERTIES_FILE} PATH)

    loadProperties("${PROPERTIES_FILE}")
    if(NOT NAME)
        message(FATAL_ERROR "${PROPERTIES_FILE}: Project NAME can not be empty.")
    endif()

    list(FIND DEPENDENCIES ${NAME} DEP_LOOP)
    if(DEP_LOOP GREATER -1)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project can not be its own dependency.")
    endif()

    list(FIND REQUIREMENTS ${NAME} REQ_LOOP)
    if(REQ_LOOP GREATER -1)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project can not be its own requirement.")
    endif()

    if(${NAME}_DIR)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project NAME must be unique in the workspace. ${NAME} already defined there : ${${NAME}_DIR}")
    endif()

    string( TOUPPER "${TYPE}" TYPE )

    set(${NAME}_DEPENDENCIES "${DEPENDENCIES}")
    set(${NAME}_REQUIREMENTS "${REQUIREMENTS}")
    set(${NAME}_DISABLE_PCH  "${DISABLE_PCH}")
    set(${NAME}_CONAN_DEPS   "${CONAN_DEPS}")

    # Get the pch target, test the existence of type variable to exclude unbuilt projects
    if(ENABLE_PCH AND TYPE AND NOT ${NAME}_DISABLE_PCH)
        set(${NAME}_PROJECT_DIR "${PROJECT_DIR}")

        getPchTarget(${NAME} ${PROJECT_DIR} ${TYPE} ${USE_PCH_FROM_TARGET})

        # Append the pch target as a dependency (duplicate will be stripped later if it was already a dependency)
        if(NOT ${${NAME}_PCH_TARGET} STREQUAL ${NAME})
            list(APPEND ${NAME}_REQUIREMENTS ${${NAME}_PCH_TARGET})
        endif()
    endif()

    set(${NAME}_VERSION "${VERSION}")
    set(${NAME}_DIR "${PROJECT_DIR}")

    if(TYPE STREQUAL "APP")
        set(${NAME}_UNIQUE "${UNIQUE}")
    endif()

    if( TYPE STREQUAL "BUNDLE" OR TYPE STREQUAL "APP" AND START)
        set(${NAME}_START "${START}")
    endif()

    set(${NAME}_START_BEFORE "${START_BEFORE}")

    list(APPEND PROJECT_LIST ${NAME})

endforeach()

# Now find all dependencies if we restrict the build to some projects
if(PROJECTS_TO_BUILD)
    set(PROJECT_LIST ${PROJECTS_TO_BUILD})
    findAllDependencies("${PROJECT_LIST}" PROJECT_LIST)

    foreach(PROJECT ${PROJECT_LIST})
        set(${PROJECT}_INSTALL 1)
    endforeach()
    message(STATUS "Project configuration is restricted to : ${PROJECTS_TO_BUILD}")
elseif(BUILD_SDK)
    foreach(PROJECT ${PROJECT_LIST})
        set(${PROJECT}_INSTALL 1)
    endforeach()
endif()

# Clear all previous tests from project list
findTests("${PROJECT_LIST}" "" PROJECT_TESTS)
if(PROJECT_TESTS)
    list(REMOVE_ITEM PROJECT_LIST ${PROJECT_TESTS})
endif()

# Append tests to the project list
if(BUILD_TESTS)
    findTests("${PROJECT_LIST}" "${TESTS_FILTER}" PROJECT_TESTS)
    list(APPEND PROJECT_LIST ${PROJECT_TESTS})
endif()

# Find and install Conan packages
if(USE_CONAN)
    findConanDeps("${PROJECT_LIST}" CONAN_DEPS_LIST)
    installConanDeps("${CONAN_DEPS_LIST}")
    add_definitions(${CONAN_DEFINES})
endif()

# Discover PCL only once as it takes a while and cache results...
add_subdirectory(${FWCMAKE_RESOURCE_PATH}/pcl)

########################################################################################################################
# Second projects scan where we actually perform the add_subdirectory() for each project and its dependencies
########################################################################################################################
if(EXTERNAL_LIBRARIES OR USE_SYSTEM_LIB OR USE_CONAN)
    set(APP_TO_BUILD)
    foreach(PROJECT ${PROJECT_LIST})
        addProject(${PROJECT})
    endforeach()
else()
    message("EXTERNAL_LIBRARIES variable is missing. Please, specify external libraries location to generate CMake projects.")
endif()


########################################################################################################################
# Export and install targets
########################################################################################################################

if(BUILD_SDK)
    # Create the SightConfig.cmake file
    set(FWCONFIG_FILE ${CMAKE_SOURCE_DIR}/CMake/build/SightConfig.cmake.in)

    configure_file(${FWCONFIG_FILE} "${CMAKE_BINARY_DIR}/cmake/SightConfig.cmake" @ONLY)

    # Create the SightConfigVersion file
    set(SIGHT_VERSION 18.0)

    write_basic_package_version_file(
      "${CMAKE_BINARY_DIR}/cmake/SightConfigVersion.cmake"
      VERSION ${SIGHT_VERSION}
      COMPATIBILITY AnyNewerVersion
    )

    if(USE_CONAN)
        file(APPEND "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
             "set(USE_CONAN 1)\n"
             "set(CONAN_DEFINES ${CONAN_DEFINES})\n")
    endif()

    # Install the SightConfig.cmake and SightConfigVersion.cmake
    install(
      FILES
        "${CMAKE_BINARY_DIR}/cmake/SightConfig.cmake"
        "${CMAKE_BINARY_DIR}/cmake/SightConfigVersion.cmake"
        "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/build/macros.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/build/conan.cmake"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}
      COMPONENT
        dev
    )

    # Install some files needed for the build
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/CMake/build/configure_file.cmake"
        "${FWCMAKE_RESOURCE_PATH}/cppunit/cppunit_main.cpp"
        "${CMAKE_SOURCE_DIR}/CMake/build/config.hpp.in"
        "${CMAKE_SOURCE_DIR}/CMake/build/plugin_config.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/build/plugin_config_command.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/build/plugin.xml.in"
        "${CMAKE_SOURCE_DIR}/CMake/build/profile_config.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/build/profile.xml.in"
        "${CMAKE_SOURCE_DIR}/CMake/build/registerServices.cpp.in"
        "${CMAKE_SOURCE_DIR}/CMake/build/spyLogLevel.hpp.in"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/build
      COMPONENT
        dev
    )

    # Install some files needed for the install
    install(
      FILES
        "${CMAKE_SOURCE_DIR}/CMake/install/generic_install.cmake"
        "${CMAKE_SOURCE_DIR}/CMake/install/helper.cmake"
      DESTINATION
        ${FWCONFIG_PACKAGE_LOCATION}/install
      COMPONENT
        dev
    )

    if(APPLE)
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/osx_install.cmake"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install
          COMPONENT
            dev
        )
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/macos/rc/app.icns"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/macos/rc
          COMPONENT
            dev
        )
        install(
          FILES
          "${CMAKE_SOURCE_DIR}/CMake/install/macos/osx_install_name_tool.py"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/macos/
          COMPONENT
            dev
        )
        install(
          FILES
          "${CMAKE_SOURCE_DIR}/CMake/build/linux/template.sh.in"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/build/linux
          COMPONENT
            dev
        )
    elseif(WIN32)
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/win_install.cmake"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install
          COMPONENT
            dev
        )
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/template.bat.in"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/setpath.bat.in"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/windows_fixup.cmake.in"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/windows
          COMPONENT
            dev
        )
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/NSIS.InstallOptions.ini.in"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/NSIS.template.in"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/
          COMPONENT
            dev
        )
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/rc/banner_nsis.bmp"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/rc/dialog_nsis.bmp"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/rc/app.ico"
            "${CMAKE_SOURCE_DIR}/CMake/install/windows/NSIS/rc/license.rtf"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/windows/NSIS/rc/
          COMPONENT
            dev
        )
    elseif(UNIX)
        install(
          FILES
          "${CMAKE_SOURCE_DIR}/CMake/build/linux/template.sh.in"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/build/linux
          COMPONENT
            dev
        )
        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/linux_install.cmake"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install
          COMPONENT
            dev
        )

        install(
          FILES
            "${CMAKE_SOURCE_DIR}/CMake/install/linux/template.sh.in"
            "${CMAKE_SOURCE_DIR}/CMake/install/linux/linux_fixup.cmake.in"
          DESTINATION
            ${FWCONFIG_PACKAGE_LOCATION}/install/linux
          COMPONENT
            dev
        )
    endif()
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

# Eclipse project
option(ECLIPSE_PROJECT "Generate eclipse project" OFF)
if(ECLIPSE_PROJECT)
    include(${FWCMAKE_RESOURCE_PATH}eclipse/eclipse_generator.cmake)
    eclipseGenerator(${PROJECT_LIST})
endif()

########################################################################################################################
# Packaging for the SDK
########################################################################################################################
if(BUILD_SDK)
    set(FWPROJECT_NAME sight)
    set(VERSION ${SIGHT_VERSION})
    generic_install()
endif()
