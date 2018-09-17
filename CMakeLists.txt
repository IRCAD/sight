cmake_minimum_required (VERSION 3.1)

project (fw4spl)

enable_testing()

include(CheckVariableExists)
include(CMakeParseArguments)
include(GNUInstallDirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/profile_config.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/plugin_config.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/generic_install.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/helper.cmake)

option(USE_CONAN "Use Conan package manager for dependencies" ON)
if(USE_CONAN)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/conan.cmake)
endif()

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

set(BUILD_SHARED_LIBS ON)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(SEND_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} (required Debug, Release, RelWithDebInfo or MinSizeRel)")
endif()
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "List of supported configurations." FORCE)
endif()

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER automoc)

if(MSVC AND ENABLE_PCH)
    # Store debug information in the .obj file instead of a PDB.
    string(REGEX REPLACE "/Z[iI]" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "/Z[iI]" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    # Force set debug compile flags.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Debug flags" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "RelWithDebInfo flags" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")  #racy backward compatibility

if(NOT USE_CONAN)
    option(USE_SYSTEM_LIB "Use system libraries to compile fw4spl" OFF)
endif()

if(NOT USE_SYSTEM_LIB AND NOT USE_CONAN)
    set(EXTERNAL_LIBRARIES CACHE PATH "External libraries location")
endif()

set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

# Append our 'FindPackages.cmake' to CMAKE_MODULE_PATH and include BinPkgs config file
if(EXTERNAL_LIBRARIES)

    list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_FIND_ROOT_PATH ${EXTERNAL_LIBRARIES})
    include(${EXTERNAL_LIBRARIES}/lib/cmake/BinPkgsConfig.cmake)
    include(${EXTERNAL_LIBRARIES}/lib/cmake/find_sse.cmake)

    # This serves two purposes:
    # - The CUDA bundles/libs in FW4SPL projects will automatically find a working CUDA toolkit.
    # - Dependencies using CUDA such as OpenCV can find the toolkit they were compiled with, they might otherwise pick
    #   another one if you have several toolkits installed, which would cause an error.
    set(CUDA_TOOLKIT_ROOT_DIR ${BINPKGS_CUDA_TOOLKIT_ROOT_DIR} CACHE PATH
        "Version of the CUDA toolkit used for this build, if any. Read-only."
        FORCE)

elseif(USE_SYSTEM_LIB)

    list(APPEND CMAKE_PREFIX_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/modules)
    list(APPEND CMAKE_FIND_ROOT_PATH ${FWCMAKE_RESOURCE_PATH}/modules)

endif()


set(CREATE_SUBPROJECTS OFF CACHE BOOL "Create a project for each subproject")

# Tests build / run options
set(BUILD_TESTS ON CACHE BOOL "Configures projects associated tests (<project>Test projects)")
set(TESTS_XML_OUTPUT OFF CACHE BOOL "Tests will generate an xml output, suitable for CI integration")
mark_as_advanced(TESTS_XML_OUTPUT)
set(TESTS_FILTER "" CACHE STRING "Allows to only build/run tests whose path contains the filter string.")
mark_as_advanced(TESTS_FILTER)

set(SET_API_VERSION ON)

# Automatic detection of additional repositories using .fw4spl files
file(GLOB ADDITIONAL_REPOSITORIES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../*/.fw4spl)

foreach(REPOSITORY ${ADDITIONAL_REPOSITORIES_FILES})
    file(READ ${REPOSITORY} REPO_RAW_NAME)
    string(STRIP ${REPO_RAW_NAME} REPO_RAW_NAME)
    string(TOUPPER ${REPO_RAW_NAME} REPO_NAME)

    if(NOT ${REPO_NAME} STREQUAL "FW4SPL")

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

if(WIN32)
    set(BUNDLE_LIB_PREFIX "${CMAKE_INSTALL_BINDIR}" CACHE PATH "Relative path where the bundles libraries are installed.")
else()
    set(BUNDLE_LIB_PREFIX "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Relative path where the bundles libraries are installed.")
endif()

set(BUNDLE_RC_PREFIX "${CMAKE_INSTALL_DATADIR}" CACHE PATH "Relative path where the bundles resources are installed.")

mark_as_advanced(BUNDLE_LIB_PREFIX)
mark_as_advanced(BUNDLE_RC_PREFIX)

# Set warning level and function export visibility on Unix
if(UNIX)
    if(APPLE)
        # actually the flag -fvisibility=hidden makes application crash on OSX
        set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
        set(CMAKE_MACOSX_RPATH ON)
    else()
        set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility=hidden")
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

if(BINPKGS_HAS_SSE)
    message(STATUS "SSE support detected in the dependencies, enabling SSE.")
    # Build with SSE support if possible
    fwFindSSE()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SSE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSE_FLAGS}")
    add_definitions("${SSE_DEFINITIONS}")
endif()

set(LIBRARY_OUTPUT_DIR lib)

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

# require C++11 standard by default
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(groupMaker FWPROJECT_NAME)
    file(GLOB_RECURSE PRJ_SOURCES "${${FWPROJECT_NAME}_DIR}/*")
    foreach(SRC ${PRJ_SOURCES})
        string(REGEX REPLACE ${${FWPROJECT_NAME}_DIR} "" REL_DIR "${SRC}")
        string(REGEX REPLACE "[\\\\/][^\\\\/]*$" "" REL_DIR "${REL_DIR}")
        string(REGEX REPLACE "^[\\\\/]" "" REL_DIR "${REL_DIR}")
        string(REPLACE "/" "\\" REL_DIR "${REL_DIR}")
        source_group("${REL_DIR}" FILES ${SRC})
    endforeach()

    file(GLOB_RECURSE PRJ_BUILD_SOURCES "${${FWPROJECT_NAME}_BUILD_DIR}/*.cpp" "${${FWPROJECT_NAME}_BUILD_DIR}/*.hpp")
    foreach(SRC ${PRJ_BUILD_SOURCES})
        source_group("genFiles" FILES ${SRC})
    endforeach()
endmacro()

macro(initProject PRJNAME )
    if(CREATE_SUBPROJECTS)
        project( ${PRJNAME} )
    endif()
    set(FWPROJECT_NAME ${PRJNAME})
    set(PRJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set(${FWPROJECT_NAME}_HEADERS)
    set(${FWPROJECT_NAME}_SOURCES)

    set(SUBDIRS ${ARGN})
    list(LENGTH SUBDIRS NB_SUBDIRS)
    if(NB_SUBDIRS EQUAL 0)
        list(APPEND SUBDIRS ${PRJ_SOURCE_DIR})
    endif()

    set(${FWPROJECT_NAME}_INCLUDE_DIR)

    foreach(SUBDIR ${SUBDIRS})
        list(APPEND ${FWPROJECT_NAME}_INCLUDE_DIR ${SUBDIR}/include)

        file(GLOB_RECURSE HEADERS ${SUBDIR}/include/*)
        file(GLOB_RECURSE SOURCES ${SUBDIR}/src/*)

        list(APPEND ${FWPROJECT_NAME}_HEADERS ${HEADERS})
        list(APPEND ${FWPROJECT_NAME}_SOURCES ${SOURCES})
    endforeach()

    set (${FWPROJECT_NAME}_INCLUDE_DIR ${${FWPROJECT_NAME}_INCLUDE_DIR} PARENT_SCOPE)
    set (${FWPROJECT_NAME}_DIR     ${CMAKE_CURRENT_SOURCE_DIR}  PARENT_SCOPE)
    set (${FWPROJECT_NAME}_BUILD_DIR ${CMAKE_BINARY_DIR}/${FWPROJECT_NAME})
    set (${FWPROJECT_NAME}_BUILD_DIR ${${FWPROJECT_NAME}_BUILD_DIR} PARENT_SCOPE)

    set (${FWPROJECT_NAME}_HEADERS ${${FWPROJECT_NAME}_HEADERS} PARENT_SCOPE)
    set (${FWPROJECT_NAME}_SOURCES ${${FWPROJECT_NAME}_SOURCES} PARENT_SCOPE)

    file(GLOB_RECURSE ${FWPROJECT_NAME}_RC_FILES "${PRJ_SOURCE_DIR}/rc/*" "${PRJ_SOURCE_DIR}/tu/rc/*")
    set(${FWPROJECT_NAME}_RC_FILES ${${FWPROJECT_NAME}_RC_FILES} PARENT_SCOPE)
    set_source_files_properties(${${FWPROJECT_NAME}_RC_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    file(GLOB ${FWPROJECT_NAME}_CMAKE_FILES "${PRJ_SOURCE_DIR}/*.txt" "${PRJ_SOURCE_DIR}/*.cmake")
    set(${FWPROJECT_NAME}_CMAKE_FILES ${${FWPROJECT_NAME}_CMAKE_FILES} PARENT_SCOPE)
    set_source_files_properties(${${FWPROJECT_NAME}_CMAKE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    if (APPLE)
        set_source_files_properties(${${FWPROJECT_NAME}_RC_FILES} PROPERTIES XCODE_LAST_KNOWN_FILE_TYPE YES)
        set_source_files_properties(${${FWPROJECT_NAME}_CMAKE_FILES} PROPERTIES XCODE_LAST_KNOWN_FILE_TYPE YES)
    endif()

    groupMaker(${FWPROJECT_NAME})
endmacro()


macro(setVersion FWPROJECT_NAME PROJECT_VERSION)
    set(${FWPROJECT_NAME}_VERSION ${PROJECT_VERSION})
    set(${FWPROJECT_NAME}_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_FULLNAME ${FWPROJECT_NAME}-${PROJECT_VERSION})
    set(${FWPROJECT_NAME}_FULLNAME ${FWPROJECT_NAME}-${PROJECT_VERSION} PARENT_SCOPE)
endmacro()

macro(configureProject FWPROJECT_NAME PROJECT_VERSION)
    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})
    string(REGEX MATCH "^[^.]+" API_VERSION ${PROJECT_VERSION})

    if(SET_API_VERSION)
        set_target_properties(${FWPROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${API_VERSION})
    endif()

    set_target_properties(${FWPROJECT_NAME} PROPERTIES DEFINE_SYMBOL ${PROJECT_NAME_UPCASE}_EXPORTS)

    target_compile_definitions(${FWPROJECT_NAME} PRIVATE ${PROJECT_NAME_UPCASE}_VER="${PROJECT_VERSION}")

    get_target_property(TARGET_TYPE ${FWPROJECT_NAME} TYPE)

    if (NOT ${TARGET_TYPE} MATCHES "EXECUTABLE")
        set(${FWPROJECT_NAME}_LIBRARY "$<TARGET_FILE:${FWPROJECT_NAME}>")
        set(${FWPROJECT_NAME}_LIBRARY ${${FWPROJECT_NAME}_LIBRARY} PARENT_SCOPE)
    endif()

    target_include_directories(${FWPROJECT_NAME} PUBLIC ${${FWPROJECT_NAME}_INCLUDE_DIR})

    set(SPYLOG_LEVEL_${FWPROJECT_NAME} "${SPYLOG_LEVEL}" CACHE STRING "${FWPROJECT_NAME}'s Log level" )
    set_property(CACHE SPYLOG_LEVEL_${FWPROJECT_NAME} PROPERTY STRINGS ${SPYLOG_LEVEL_VALUES} )
    mark_as_advanced(SPYLOG_LEVEL_${FWPROJECT_NAME})

    if( SPYLOG_LEVEL_MAP_${SPYLOG_LEVEL_${FWPROJECT_NAME}} )
        if(ENABLE_PCH AND (${TARGET_TYPE} MATCHES "LIBRARY" OR ${TARGET_TYPE} MATCHES "BUNDLE" )
           AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
            target_compile_definitions(${FWPROJECT_NAME}
                PRIVATE SPYLOG_LEVEL_${FWPROJECT_NAME}=${SPYLOG_LEVEL_MAP_${SPYLOG_LEVEL_${FWPROJECT_NAME}}})
        else()
            target_compile_definitions(${FWPROJECT_NAME}
                PRIVATE SPYLOG_LEVEL=${SPYLOG_LEVEL_MAP_${SPYLOG_LEVEL_${FWPROJECT_NAME}}}
                PRIVATE SPYLOG_NO_INCLUDE=1)
        endif()
    else()
        message(SEND_ERROR "${SPYLOG_LEVEL_${FWPROJECT_NAME}} is not a valid value for SPYLOG_LEVEL_${FWPROJECT_NAME}" )
    endif()
    unset(SPYLOG_VALID_VALUE)

endmacro()


macro(createResourcesTarget TARGET RES_DIR TARGET_DIR)
    file(GLOB_RECURSE RESOURCES_FILES "${RES_DIR}/*")
    set(CREATED_RESOURCES_LIST)
    foreach(RESOURCE_FILE ${RESOURCES_FILES})
        file(RELATIVE_PATH REL_PATH "${RES_DIR}" "${RESOURCE_FILE}")
        if ("${TARGET_DIR}/${REL_PATH}" MATCHES "^.*\\.(txt|xml|cfg)$")
            set(COPY_COMMAND ${CMAKE_COMMAND}
                             -DIN_FILE="${RES_DIR}/${REL_PATH}"
                             -DOUT_FILE="${TARGET_DIR}/${REL_PATH}"
                             -DFWPROJECT_NAME="${FWPROJECT_NAME}"
                             -DPROJECT_VERSION="${${FWPROJECT_NAME}_VERSION}"
                             -P ${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake
            )
            set(COPY_DEPENDS "${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake" )
        else()

            set(COPY_COMMAND ${CMAKE_COMMAND} -E copy "${RES_DIR}/${REL_PATH}" "${TARGET_DIR}/${REL_PATH}")
            set(COPY_DEPENDS )
        endif()

        add_custom_command(OUTPUT "${TARGET_DIR}/${REL_PATH}"
            COMMAND ${COPY_COMMAND}
            DEPENDS "${RES_DIR}/${REL_PATH}" ${COPY_DEPENDS}
            )
        LIST (APPEND CREATED_RESOURCES_LIST "${TARGET_DIR}/${REL_PATH}")

    endforeach()

    add_custom_target("${TARGET}" ALL DEPENDS ${CREATED_RESOURCES_LIST} )

    # Adds project into folder rc
    set_target_properties("${TARGET}" PROPERTIES FOLDER "rc")

    unset(CREATED_RESOURCES_LIST)
endmacro()

macro(createResourcesInstallTarget CONFIGURED_FILES_DIR DESTINATION)
    install(DIRECTORY "${CONFIGURED_FILES_DIR}/" DESTINATION "${DESTINATION}/")
endmacro()


macro(fwExec FWPROJECT_NAME PROJECT_VERSION)
    set(options CONSOLE)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(FWEXEC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    initProject( ${FWPROJECT_NAME} )

    set(${FWPROJECT_NAME}_TYPE "EXECUTABLE")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})

    if(WIN32 AND NOT ${FWEXEC_CONSOLE})
        set(GUI_TYPE WIN32)
    else()
        add_definitions(-D_CONSOLE)
    endif()

    add_executable(${FWPROJECT_NAME} ${GUI_TYPE}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${${FWPROJECT_NAME}_RC_FILES}
        ${${FWPROJECT_NAME}_CMAKE_FILES})

    configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
        endif()
    endif()

    if(${FWPROJECT_NAME}_INSTALL)
        install(
            TARGETS ${FWPROJECT_NAME}
            RUNTIME DESTINATION bin
            OPTIONAL
            )
    endif()

    # Adds project into folder exec
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "exec")
endmacro()

macro(fwCppunitTest FWPROJECT_NAME)
    set(options)
    set(oneValueArgs BUNDLE WORKING_DIRECTORY)
    set(multiValueArgs)
    cmake_parse_arguments(fwCppunitTest "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    configure_file(
        "${FWCMAKE_RESOURCE_PATH}/cppunit/cppunit_main.cpp"
        "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/src/cppunit_main.cpp"
        IMMEDIATE @ONLY)

    initProject( ${FWPROJECT_NAME} tu )

    set(${FWPROJECT_NAME}_TYPE "TEST")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} 0.0)

    string(REGEX REPLACE "Test$" "" DIRNAME "${FWPROJECT_NAME}")
    set(TU_NAME "tu_exec_${DIRNAME}-0.0")

    add_executable(${FWPROJECT_NAME}
        ${fwCppunitTest_UNPARSED_ARGUMENTS}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/src/cppunit_main.cpp
        ${${FWPROJECT_NAME}_RC_FILES}
        ${${FWPROJECT_NAME}_CMAKE_FILES})

    if(fwCppunitTest_BUNDLE)
        add_definitions(-DBUNDLE_TEST_PROFILE=\"${BUNDLE_RC_PREFIX}/${TU_NAME}/profile.xml\")
    endif()

    configureProject( ${FWPROJECT_NAME} 0.0 )

    if(EXISTS "${PRJ_SOURCE_DIR}/tu/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${TU_NAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/tu/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${BUNDLE_RC_PREFIX}/${TU_NAME}" )
        endif()
    endif()

    if(fwCppunitTest_WORKING_DIRECTORY)
        set(fwCppunitTest_WORKING_DIRECTORY WORKING_DIRECTORY ${fwCppunitTest_WORKING_DIRECTORY})
    endif()

    set(${FWPROJECT_NAME}_EXECUTABLE "$<TARGET_FILE:${FWPROJECT_NAME}>")

    if(TESTS_XML_OUTPUT)
        add_test(NAME ${FWPROJECT_NAME} ${fwCppunitTest_WORKING_DIRECTORY} COMMAND ${${FWPROJECT_NAME}_EXECUTABLE} --xml)
        set_tests_properties(${FWPROJECT_NAME} PROPERTIES TIMEOUT 240)
    else()
        add_test(NAME ${FWPROJECT_NAME} ${fwCppunitTest_WORKING_DIRECTORY} COMMAND ${${FWPROJECT_NAME}_EXECUTABLE})
    endif()

    # Adds project into folder test
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "test")

    if(MSVC_IDE)
        # create the launch config for the current test
        set(LAUNCHER "${CMAKE_BINARY_DIR}/bin/${FWPROJECT_NAME}")
        set(PROFILE "")
        set(WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
        include(${FWCMAKE_RESOURCE_PATH}/install/win_install.cmake)
        if(NOT DEBUGGER_ENVIRONMENT)
            findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)
            set(DEBUGGER_ENVIRONMENT ${EXTERNAL_LIBRARIES_DIRECTORIES}
                CACHE INTERNAL "List of all folders containing external libraries")
        endif()
        if(CMAKE_CL_64)
            set(PLATFORM "x64")
        else()
            set(PLATFORM "Win32")
        endif()
        configure_file(
            "${CMAKE_SOURCE_DIR}/CMake/build/project.vcxproj.user.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/${FWPROJECT_NAME}.vcxproj.user"
            IMMEDIATE @ONLY)
    endif()
endmacro()

macro(fwLib FWPROJECT_NAME PROJECT_VERSION)
    initProject( ${FWPROJECT_NAME} )

    set(${FWPROJECT_NAME}_TYPE "LIBRARY")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)

    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})

    if(ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header_cpp(${FWPROJECT_NAME})
        endif()
        set(${FWPROJECT_NAME}_PCH_LIB $<TARGET_OBJECTS:${${FWPROJECT_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    add_library(${FWPROJECT_NAME} ${ARGN}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${${FWPROJECT_NAME}_RC_FILES}
        ${${FWPROJECT_NAME}_CMAKE_FILES}
        ${${FWPROJECT_NAME}_PCH_LIB})

    configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
        endif()

    endif()

    if(${FWPROJECT_NAME}_INSTALL)
        install(
            TARGETS ${FWPROJECT_NAME}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            OPTIONAL NAMELINK_SKIP
            )
    endif()

    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    # create the config.hpp for the current library
    configure_file(
        "${CMAKE_SOURCE_DIR}/CMake/build/config.hpp.in"
        "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/config.hpp"
        IMMEDIATE @ONLY)

    target_include_directories(${FWPROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/")

    # Adds project into folder lib
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "lib")

    if(ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        # create the config.hpp for the current library
        configure_file(
            "${CMAKE_SOURCE_DIR}/CMake/build/spyLogLevel.hpp.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/spyLogLevel.hpp"
            IMMEDIATE @ONLY)

        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header(${FWPROJECT_NAME} include/${FWPROJECT_NAME}/pch.hpp)
            if(VERBOSE_PCH)
                message(STATUS "Use custom precompiled header")
            endif()
        else()
            use_precompiled_header(${FWPROJECT_NAME} ${${FWPROJECT_NAME}_PCH_TARGET})
            if(VERBOSE_PCH)
                message(STATUS "Use ${${FWPROJECT_NAME}_PCH_TARGET} precompiled header")
            endif()
        endif()
    endif()

endmacro()

macro(fwBundle FWPROJECT_NAME PROJECT_VERSION)
    initProject( ${FWPROJECT_NAME} )

    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})

    if(ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header_cpp(${FWPROJECT_NAME})
        endif()
        set(${FWPROJECT_NAME}_PCH_LIB $<TARGET_OBJECTS:${${FWPROJECT_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    set(BUNDLE_DIR "${CMAKE_BINARY_DIR}/${BUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

    if(EXISTS "${PRJ_SOURCE_DIR}/src")

        add_library(${FWPROJECT_NAME} ${ARGN}
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            ${${FWPROJECT_NAME}_PCH_LIB}
        )

        # create the custom command that may generate the plugin.xml and the registerServices.cpp file
        if(NOT EXISTS "${${FWPROJECT_NAME}_DIR}/rc/plugin.xml" )
            target_sources( ${FWPROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/registerServices.cpp" )
            set_source_files_properties("${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/registerServices.cpp" PROPERTIES
                GENERATED TRUE
                SKIP_AUTOMOC ON)

            plugin_setup("${FWPROJECT_NAME}" "${${FWPROJECT_NAME}_HEADERS}")
        endif()

        configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

        set_target_properties(${FWPROJECT_NAME} PROPERTIES OUTPUT_NAME ${${FWPROJECT_NAME}_FULLNAME})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUNDLE_DIR})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUNDLE_DIR})
        # Fixed path for multi-config builds (e.g. msvc)
        foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
            string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
            set_target_properties(${FWPROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
            set_target_properties(${FWPROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
            set_target_properties(${FWPROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
        endforeach()

        if(${FWPROJECT_NAME}_INSTALL)
            qt_plugins_setup() # search and setup qt plugins for each bundles
            install(
                TARGETS ${FWPROJECT_NAME}
                RUNTIME DESTINATION ${BUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}
                LIBRARY DESTINATION ${BUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}
                OPTIONAL NAMELINK_SKIP
                )
        endif()

        # create the config.hpp for the current library
        configure_file(
            "${CMAKE_SOURCE_DIR}/CMake/build/config.hpp.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/config.hpp"
            IMMEDIATE @ONLY)

        target_include_directories(${FWPROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/")

        if(ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
            # create the config.hpp for the current library
            configure_file(
                "${CMAKE_SOURCE_DIR}/CMake/build/spyLogLevel.hpp.in"
                "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/spyLogLevel.hpp"
                IMMEDIATE @ONLY)

            if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
                add_precompiled_header(${FWPROJECT_NAME} include/${FWPROJECT_NAME}/pch.hpp)
                if(VERBOSE_PCH)
                    message(STATUS "Use custom precompiled header")
                endif()
            else()
                use_precompiled_header(${FWPROJECT_NAME} ${${FWPROJECT_NAME}_PCH_TARGET})
                if(VERBOSE_PCH)
                    message(STATUS "Use ${${FWPROJECT_NAME}_PCH_TARGET} precompiled header")
                endif()
            endif()
        endif()
    else()
        add_custom_target(${FWPROJECT_NAME} SOURCES
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES})
    endif()

    # Adds project into folder bundle or apps
    if(TYPE STREQUAL "APP")
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "app")
        if(MSVC_IDE)
            # create the launch config for the current app
            set(LAUNCHER "${CMAKE_BINARY_DIR}/bin/fwlauncher.exe")
            set(PROFILE "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/profile.xml")
            set(WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
            include(${FWCMAKE_RESOURCE_PATH}/install/win_install.cmake)
            if(NOT DEBUGGER_ENVIRONMENT)
                findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)
                set(DEBUGGER_ENVIRONMENT ${EXTERNAL_LIBRARIES_DIRECTORIES}
                    CACHE INTERNAL "List of all folders containing external libraries")
            endif()
            if(CMAKE_CL_64)
                set(PLATFORM "x64")
            else()
                set(PLATFORM "Win32")
            endif()
            configure_file(
                "${CMAKE_SOURCE_DIR}/CMake/build/project.vcxproj.user.in"
                "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/${FWPROJECT_NAME}.vcxproj.user"
                IMMEDIATE @ONLY)
        endif()
        if(UNIX)
            # Install shortcut
            string(TOLOWER ${FWPROJECT_NAME} APP_NAME)

            set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
            set(LAUNCHER "fwlauncher-${fwlauncher_VERSION}")
            set(PROFILE_PATH "${${FWPROJECT_NAME}_FULLNAME}/profile.xml")

            configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
            file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        endif()
    else()
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "bundle")
    endif()

    set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )
    endif()

    if(${FWPROJECT_NAME}_INSTALL)
        createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${BUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
    endif()
endmacro()

# Include the projects in parameter and export them.
# Compiling warnings will not be reported (because of SYSTEM).
macro(fwForwardInclude)
    target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${ARGV})
endmacro()

macro(fwForwardLink)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ${ARGV})
endmacro()

# Include the projects in parameter but do not export them.
# Compiling warnings will not be reported (because of SYSTEM).
macro(fwInclude)
    target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${ARGV})
endmacro()

macro(fwLink)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${ARGV})
endmacro()


# Defines project's linked dependencies on others projects
# example :
#     fwUse( fwCore fwData )
# WARNING : some part of this cmake file relies on this macro signature

macro(fwDefineDependencies)

    set(${NAME}_DEPENDENCIES)
    set(${NAME}_BUNDLE_DEPENDENCIES)

    foreach(PROJECT ${ARGV})
        list(APPEND ${NAME}_DEPENDENCIES ${PROJECT})
        list(APPEND ${NAME}_${${PROJECT}_TYPE}_DEPENDENCIES ${PROJECT})
    endforeach()

    set(${NAME}_DEPENDENCIES ${${NAME}_DEPENDENCIES} PARENT_SCOPE)
    set(${NAME}_BUNDLE_DEPENDENCIES ${${NAME}_BUNDLE_DEPENDENCIES} PARENT_SCOPE)

endmacro()

# Defines project's linked dependencies on others projects
# example :
#     fwUse( fwCore fwData )
# WARNING : some part of this cmake file relies on this macro signature

macro(fwUse)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ${ARGV})
endmacro()



# Defines project's requirements on others projects
# example :
#     fwReq( ioVTK ioITK )
# WARNING : some part of this cmake file relies on this macro signature
macro(fwReq)
    add_dependencies(${FWPROJECT_NAME} ${ARGV})

    set(${FWPROJECT_NAME}_REQUIREMENTS)
    set(${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS)

    foreach(PROJECT ${ARGV})
        list(APPEND ${FWPROJECT_NAME}_REQUIREMENTS ${PROJECT})
        list(APPEND ${FWPROJECT_NAME}_${${PROJECT}_TYPE}_REQUIREMENTS ${PROJECT})
    endforeach()

    set(${FWPROJECT_NAME}_REQUIREMENTS ${${FWPROJECT_NAME}_REQUIREMENTS} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS ${${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS} PARENT_SCOPE)

endmacro()


function(checkMissingDependencies DIR KEYWORD RESULT)
    set(${RESULT} "" PARENT_SCOPE)
    file(READ ${DIR}/CMakeLists.txt CMAKELISTS_CONTENT)
    string(REGEX MATCH "${KEYWORD} *[^)]+" DEPENDENCIES "${CMAKELISTS_CONTENT}")
    string(REGEX REPLACE "^.*\\(" " " DEPENDENCIES "${DEPENDENCIES}")
    string(STRIP DEPENDENCIES "${DEPENDENCIES}")
    string(REGEX REPLACE "( |\n|#)+" ";" DEPENDENCIES "${DEPENDENCIES}")
    string(REGEX REPLACE "^;" "" DEPENDENCIES "${DEPENDENCIES}")
    string(REGEX REPLACE ";+" ";" DEPENDENCIES "${DEPENDENCIES}")


    set(DEPENDENCY_LIST)
    foreach(DEPENDENCY ${DEPENDENCIES})
        if(NOT ${DEPENDENCY}_TYPE)
            list(APPEND DEPENDENCY_LIST ${DEPENDENCY})
        endif()
    endforeach()
    set(${RESULT} ${DEPENDENCY_LIST} PARENT_SCOPE)
endfunction()


function(findAllDependencies FWPROJECT_NAMES RESULT_VAR)
    set(DEPENDENCY_LIST)
    set(RESULT "")
    list(APPEND DEPENDENCY_LIST ${FWPROJECT_NAMES})
    while(DEPENDENCY_LIST)

        list(GET DEPENDENCY_LIST 0 DEPENDENCY)
        list(REMOVE_AT DEPENDENCY_LIST 0 )

        if(NOT PROCESSED_${DEPENDENCY})
            list(APPEND DEPENDENCY_LIST ${${DEPENDENCY}_DEPENDENCIES})
            list(APPEND DEPENDENCY_LIST ${${DEPENDENCY}_REQUIREMENTS})
            set(PROCESSED_${DEPENDENCY} 1)
        endif()

        list(APPEND RESULT ${DEPENDENCY})
    endwhile()

    list(REMOVE_DUPLICATES RESULT)
    set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)

endfunction()

function(findTests FWPROJECTS FILTER RESULT_VAR)
    set(RESULT "")

    foreach(PROJECT ${FWPROJECTS})
        if(${PROJECT}Test_DIR AND ("${FILTER}" STREQUAL "" OR "${${PROJECT}Test_DIR}" MATCHES "${FILTER}" ))
            list(APPEND RESULT ${PROJECT}Test)
        endif()
    endforeach()

    set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

function(getPchTarget FWPROJECT_NAME PROJECT_DIR TYPE)

    if(ARGN)
        # Use pch from an another target
        set(${FWPROJECT_NAME}_PCH_TARGET ${ARGN} PARENT_SCOPE)
    elseif(EXISTS "${PROJECT_DIR}/include/${FWPROJECT_NAME}/pch.hpp")
        # Custom pch
        set(${FWPROJECT_NAME}_PCH_TARGET ${FWPROJECT_NAME} PARENT_SCOPE)
    else()
        # Default pch
        if( TYPE STREQUAL "BUNDLE" )
            set(${FWPROJECT_NAME}_PCH_TARGET pchServices PARENT_SCOPE)
        else()
            set(${FWPROJECT_NAME}_PCH_TARGET pchCore PARENT_SCOPE)
        endif()
    endif()

endfunction()

macro(loadProperties PROPERTIES_FILE)
    unset(NAME)
    unset(OPTIONS)
    unset(VERSION)
    unset(TYPE)
    unset(START)
    unset(DEPENDENCIES)
    unset(REQUIREMENTS)
    unset(CPPUNITTEST_OPTIONS)
    unset(UNIQUE)
    unset(PLATFORM)
    unset(USE_PCH_FROM_TARGET)
    unset(DISABLE_PCH)
    unset(START_BEFORE)
    unset(CONAN_DEPS)

    include("${PROPERTIES_FILE}")
endmacro()


macro(fwLoadProperties)
    loadProperties("Properties.cmake")

    string( TOUPPER "${TYPE}" TYPE )

    fwDefineDependencies( ${NAME} ${DEPENDENCIES} )

    if( TYPE STREQUAL "EXECUTABLE" )
        fwExec(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "LIBRARY" )
        fwLib(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "BUNDLE" )
        set(${NAME}_TYPE "BUNDLE")
        fwBundle(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "TEST" )
        fwCppunitTest(${NAME} "${CPPUNITTEST_OPTIONS}" "${OPTIONS}")
    elseif( TYPE STREQUAL "APP" )
        set(${NAME}_TYPE "APP")
        fwBundle(${NAME} ${VERSION} ${OPTIONS})
    endif()

    if(DEPENDENCIES)
        fwUse( ${DEPENDENCIES} )
    endif()
    if(REQUIREMENTS)
        fwReq( ${REQUIREMENTS} )
    endif()

endmacro()


macro(addProject PROJECT)
    set(PROJECT_CACHE ${ARGN})
    list(FIND PROJECT_CACHE "${PROJECT}" DEP_LOOP)
    if(DEP_LOOP GREATER -1)
        message(FATAL_ERROR "Looks like there is a dependency loop in projects: ${PROJECT_CACHE};${PROJECT}")
    endif()

    if( NOT ${PROJECT}_CONFIGURED )
        list(APPEND PROJECT_CACHE ${PROJECT})

        foreach(DEPENDENCY ${${PROJECT}_DEPENDENCIES})
            addProject( ${DEPENDENCY} "${PROJECT_CACHE}")
        endforeach()

        foreach(REQUIREMENT ${${PROJECT}_REQUIREMENTS})
            addProject( ${REQUIREMENT} "${PROJECT_CACHE}")
        endforeach()

        set(${PROJECT}_CONFIGURED 1)

        message(STATUS "Configuring ${PROJECT}: ${${PROJECT}_DIR}")
        if(${PROJECT}_DIR)
            add_subdirectory(${${PROJECT}_DIR} ${PROJECT})
        else()
            message(SEND_ERROR "<${PROJECT}> dir '' not found.")
        endif()

    endif()
    unset(PROJECT_CACHE)
endmacro()

# Macro that allows to skip the find_package(PCL) which is very slow
macro(fwQuickFindPCL)
    if(NOT PCL_FOUND)
        message(SEND_ERROR "PCL not found")
    endif()

    # This is necessary for now to run this, at least on Linux, in order to resolve the version number of the libraries,
    # .i.e from -lvtksys to -lvtksys7.1
    find_package(VTK QUIET)
endmacro()

# Parsing of 'fw-*.cmake' scripts
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


set(PROJECTS_TO_BUILD CACHE STRING
        "List of projects that will be configured for build.
        Leave empty to configure all projects"
    )

set(PROJECTS_TO_INSTALL CACHE STRING
        "List of projects for which the installation rules will be created."
    )

file(GLOB_RECURSE PROJECTS_PROPERTIES */Properties.cmake)

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

unset(PROJECT_LIST)
unset(CONAN_DEPS_LIST)
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

if(PROJECTS_TO_BUILD)
    set(PROJECT_LIST ${PROJECTS_TO_BUILD})
    findAllDependencies("${PROJECT_LIST}" PROJECT_LIST)
    message(STATUS "Project configuration is restricted to : ${PROJECTS_TO_BUILD}")
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

add_subdirectory(${FWCMAKE_RESOURCE_PATH}/pcl)

if(PROJECTS_TO_INSTALL)
    message(STATUS "Installation rules will be created for : ${PROJECTS_TO_INSTALL}")
    findAllDependencies("${PROJECTS_TO_INSTALL}" PROJECTS_TO_INSTALL)

    foreach(PROJECT ${PROJECTS_TO_INSTALL})
        list(FIND PROJECT_LIST "${PROJECT}" PROJECT_FOUND)
        if(PROJECT_FOUND GREATER -1)
            set(${PROJECT}_INSTALL 1)
        else()
            message(WARNING "Project ${PROJECT} not found, will not be installed.")
            unset(${PROJECT}_INSTALL)
        endif()
    endforeach()
    unset(PROJECT_FOUND)
endif()

if(EXTERNAL_LIBRARIES OR USE_SYSTEM_LIB OR USE_CONAN)
    set(APP_TO_BUILD)
    foreach(PROJECT ${PROJECT_LIST})
        addProject(${PROJECT})
        if(${PROJECT}_TYPE STREQUAL "APP")
            if(NOT EXISTS "${${PROJECT}_DIR}/rc/profile.xml" )
                profile_setup("${PROJECT}")
            endif()
        endif()
    endforeach()
else()
    message("EXTERNAL_LIBRARIES variable is missing. Please, specify external libraries location to generate CMake projects.")
endif()

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

