
cmake_minimum_required (VERSION 2.8.11)

project (fw4spl)

enable_testing()

include(CheckVariableExists)
include(CMakeParseArguments)


set(BUILD_SHARED_LIBS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")  #racy backward compatibility

# Set warning level and function export visibility on Unix
if(UNIX)
    if(APPLE)
        # actually the flag -fvisibility=hidden makes application crash on OSX
        set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility-ms-compat")
    else()
        set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-unused-parameter -Wno-ignored-qualifiers -fvisibility=hidden")
    endif()
    # Color for ninja and Clang on Linux and OSX
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
endif()

set(CREATE_SUBPROJECTS OFF CACHE BOOL "Create a project for each subproject")

set(BUILD_TESTS ON CACHE BOOL "Configures projects associated tests (<project>Test projects)")

set(LIBRARY_OUTPUT_DIR lib)

set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    #http://stackoverflow.com/questions/6802903/c-ifdef-mac-os-x-question
    add_definitions(-D__MACOSX__) #racy backward compatibility
    set(LIBRARY_OUTPUT_DIR Libraries)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIBRARY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIBRARY_OUTPUT_DIR})


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

# Globaly enabled c++11
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
    if(APPLE)
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --stdlib=libc++")
    endif(APPLE)
endif()

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
    endforeach(SUBDIR)

    set (${FWPROJECT_NAME}_INCLUDE_DIR ${${FWPROJECT_NAME}_INCLUDE_DIR} PARENT_SCOPE) 
    set (${FWPROJECT_NAME}_DIR     ${CMAKE_CURRENT_SOURCE_DIR}  PARENT_SCOPE)
    
    set (${FWPROJECT_NAME}_HEADERS ${${FWPROJECT_NAME}_HEADERS} PARENT_SCOPE) 
    set (${FWPROJECT_NAME}_SOURCES ${${FWPROJECT_NAME}_SOURCES} PARENT_SCOPE)
    
endmacro(initProject)


macro(setVersion FWPROJECT_NAME PROJECT_VERSION)
    set(${FWPROJECT_NAME}_VERSION ${PROJECT_VERSION} )
    set(${FWPROJECT_NAME}_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
    string(REPLACE "." "-" DASH_VERSION "${PROJECT_VERSION}")
    set(${FWPROJECT_NAME}_DASH_VERSION ${DASH_VERSION} )
    set(${FWPROJECT_NAME}_DASH_VERSION ${DASH_VERSION} PARENT_SCOPE)
endmacro()

macro(configureProject FWPROJECT_NAME PROJECT_VERSION)
    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})
    string(REGEX MATCH "^[^.]+" API_VERSION ${PROJECT_VERSION})
    set_target_properties(${FWPROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${API_VERSION})
    set_target_properties(${FWPROJECT_NAME} PROPERTIES DEFINE_SYMBOL ${PROJECT_NAME_UPCASE}_EXPORTS)

    add_definitions(-D${PROJECT_NAME_UPCASE}_VER="${DASH_VERSION}")

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
        add_definitions(-DSPYLOG_LEVEL=${SPYLOG_LEVEL_MAP_${SPYLOG_LEVEL_${FWPROJECT_NAME}}})
    else()
        message(SEND_ERROR "${SPYLOG_LEVEL_${FWPROJECT_NAME}} is not a valid value for SPYLOG_LEVEL_${FWPROJECT_NAME}" )
    endif()
    unset(SPYLOG_VALID_VALUE)

endmacro(configureProject)


macro(createResourcesTarget TARGET RES_DIR TARGET_DIR)
    file(GLOB_RECURSE RESOURCES_FILES "${RES_DIR}/*")
    set(CREATED_RESOURCES_LIST)
    foreach(RESOURCE_FILE ${RESOURCES_FILES})
        file(RELATIVE_PATH REL_PATH "${RES_DIR}" "${RESOURCE_FILE}")
        if ("${TARGET_DIR}/${REL_PATH}" MATCHES "^.*\\.(txt|xml)$")
            set(COPY_COMMAND ${CMAKE_COMMAND}
            -DIN_FILE="${RES_DIR}/${REL_PATH}" 
            -DOUT_FILE="${TARGET_DIR}/${REL_PATH}" 
            -DFWPROJECT_NAME="${FWPROJECT_NAME}"
            -DDASH_VERSION="${${FWPROJECT_NAME}_DASH_VERSION}"
            -DPROJECT_VERSION="${${FWPROJECT_NAME}_VERSION}"
            -DTARGET_TYPE="${TARGET_TYPE}"
            -P ${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake
            )
            set(COPY_DEPENDS "${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake" )
        else()
            set(COPY_COMMAND ${CMAKE_COMMAND} -E copy "${RES_DIR}/${REL_PATH}" "${TARGET_DIR}/${REL_PATH}")
            set(COPY_DEPENDS )
        endif()

        add_custom_command(OUTPUT "${TARGET_DIR}/${REL_PATH}"
            # COMMAND cmake -E copy "${RES_DIR}/${REL_PATH}" "${TARGET_DIR}/${REL_PATH}"
            COMMAND ${COPY_COMMAND}
            DEPENDS "${RES_DIR}/${REL_PATH}" ${COPY_DEPENDS}
            ) 
        LIST (APPEND CREATED_RESOURCES_LIST "${TARGET_DIR}/${REL_PATH}")
    endforeach()

    add_custom_target("${TARGET}" ALL DEPENDS ${CREATED_RESOURCES_LIST} )

    unset(CREATED_RESOURCES_LIST)
endmacro()

macro(createResourcesInstallTarget RES_DIR CONFIGURED_FILES_DIR DESTINATION)
    file(GLOB_RECURSE RESOURCES_FILES "${RES_DIR}/*")
    foreach(RESOURCE_FILE ${RESOURCES_FILES})
        file(RELATIVE_PATH REL_PATH "${RES_DIR}" "${RESOURCE_FILE}")
        get_filename_component(RC_REL_DIR "${REL_PATH}" DIRECTORY)
        install(FILES "${CONFIGURED_FILES_DIR}/${REL_PATH}" DESTINATION "${DESTINATION}/${RC_REL_DIR}" )
        unset(RC_REL_DIR)
    endforeach()
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

    add_executable(${FWPROJECT_NAME} ${GUI_TYPE} ${${FWPROJECT_NAME}_HEADERS} ${${FWPROJECT_NAME}_SOURCES} )

    configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/share/${FWPROJECT_NAME}_${DASH_VERSION}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "share/${FWPROJECT_NAME}_${DASH_VERSION}" )
        endif()
    endif()

    if(${FWPROJECT_NAME}_INSTALL)
        install(
            TARGETS ${FWPROJECT_NAME}
            RUNTIME DESTINATION bin
            OPTIONAL
            )
    endif()


endmacro(fwExec)

macro(fwCppunitTest FWPROJECT_NAME)
    set(options)
    set(oneValueArgs BUNDLE WORKING_DIRECTORY)
    set(multiValueArgs)
    cmake_parse_arguments(fwCppunitTest "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    initProject( ${FWPROJECT_NAME} tu )

    set(${FWPROJECT_NAME}_TYPE "TEST")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} 0.0)

    add_executable(${FWPROJECT_NAME}
        ${fwCppunitTest_UNPARSED_ARGUMENTS}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${FWCMAKE_RESOURCE_PATH}/cppunit/cppunit_main.cpp
        )

    if(fwCppunitTest_BUNDLE)
        add_definitions(-DBUNDLE_TEST_PROFILE=\"share/tu_exec_${fwCppunitTest_BUNDLE}_0-0/profile.xml\")
    endif()

    configureProject( ${FWPROJECT_NAME} 0.0 )

    if(EXISTS "${PRJ_SOURCE_DIR}/tu/rc")
        string(REGEX REPLACE "Test$" "" DIRNAME "${FWPROJECT_NAME}")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/share/tu_exec_${DIRNAME}_${DASH_VERSION}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/tu/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${PRJ_SOURCE_DIR}/tu/rc"
                "${${FWPROJECT_NAME}_RC_BUILD_DIR}" 
                "share/tu_exec_${DIRNAME}_${DASH_VERSION}" )
        endif()
    endif()

    if(fwCppunitTest_WORKING_DIRECTORY)
        set(fwCppunitTest_WORKING_DIRECTORY WORKING_DIRECTORY ${fwCppunitTest_WORKING_DIRECTORY})
    endif()

    set(${FWPROJECT_NAME}_EXECUTABLE "$<TARGET_FILE:${FWPROJECT_NAME}>")
    add_test(NAME ${FWPROJECT_NAME} ${fwCppunitTest_WORKING_DIRECTORY} COMMAND ${${FWPROJECT_NAME}_EXECUTABLE} )

endmacro(fwCppunitTest)

macro(fwLib FWPROJECT_NAME PROJECT_VERSION)
    initProject( ${FWPROJECT_NAME} )

    set(${FWPROJECT_NAME}_TYPE "LIBRARY")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})

    add_library(${FWPROJECT_NAME} ${ARGN} ${${FWPROJECT_NAME}_HEADERS} ${${FWPROJECT_NAME}_SOURCES} )

    configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/share/${FWPROJECT_NAME}_${DASH_VERSION}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "share/${FWPROJECT_NAME}_${DASH_VERSION}" )
        endif()

    endif()

    if(${FWPROJECT_NAME}_INSTALL)
        install(
            TARGETS ${FWPROJECT_NAME}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR}
            OPTIONAL
            )
    endif()

    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    # create the config.hpp for the current library
    configure_file(
        "${CMAKE_SOURCE_DIR}/CMake/build/config.hpp.in"
        "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/config.hpp"
        IMMEDIATE @ONLY)

    target_include_directories(${FWPROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/")

endmacro(fwLib)


macro(fwBundle FWPROJECT_NAME PROJECT_VERSION)
    initProject( ${FWPROJECT_NAME} )

    set(${FWPROJECT_NAME}_TYPE "BUNDLE")
    set(${FWPROJECT_NAME}_TYPE ${${FWPROJECT_NAME}_TYPE} PARENT_SCOPE)
    setVersion(${FWPROJECT_NAME} ${PROJECT_VERSION})

    set(BUNDLE_DIR "${CMAKE_BINARY_DIR}/Bundles/${FWPROJECT_NAME}_${DASH_VERSION}")

    if(EXISTS "${PRJ_SOURCE_DIR}/src")
        add_library( ${FWPROJECT_NAME} SHARED ${ARGN} ${${FWPROJECT_NAME}_HEADERS} ${${FWPROJECT_NAME}_SOURCES})

        configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

        set_target_properties(${FWPROJECT_NAME} PROPERTIES OUTPUT_NAME ${FWPROJECT_NAME}_${DASH_VERSION})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUNDLE_DIR})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUNDLE_DIR})

        #install(DIRECTORY "${BUNDLE_DIR}" DESTINATION VRRender.app/Bundles COMPONENT Runtime)

        if(${FWPROJECT_NAME}_INSTALL)
            install(
                TARGETS ${FWPROJECT_NAME}
                RUNTIME DESTINATION Bundles/${FWPROJECT_NAME}_${DASH_VERSION}
                LIBRARY DESTINATION Bundles/${FWPROJECT_NAME}_${DASH_VERSION}
                OPTIONAL
                )
        endif()

        # create the config.hpp for the current library
        configure_file(
            "${CMAKE_SOURCE_DIR}/CMake/build/config.hpp.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/config.hpp"
            IMMEDIATE @ONLY)

        target_include_directories(${FWPROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/")
    else()
        add_custom_target(${FWPROJECT_NAME})
    endif()

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${BUNDLE_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL)
            createResourcesInstallTarget( "${PRJ_SOURCE_DIR}/rc" "${BUNDLE_DIR}" Bundles/${FWPROJECT_NAME}_${DASH_VERSION} )
        endif()
    endif()

endmacro(fwBundle)

# Include the projects in parameter and their public dependencies and export them.
# Public dependencies of a project are those defined either with fwForwardInclude or fwUseForwardInclude.
macro(fwUseForwardInclude)
    foreach(PROJECT ${ARGV})
        get_target_property(PROJECT_INCLUDE_DIRECTORIES ${PROJECT} INTERFACE_INCLUDE_DIRECTORIES)
        list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRECTORIES)
        target_include_directories(${FWPROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIRECTORIES})
    endforeach(PROJECT)
endmacro(fwUseForwardInclude)

macro(fwUseForwardLink)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ${ARGV})
endmacro(fwUseForwardLink)

macro(fwUseForward)
    fwUseForwardInclude(${ARGV})
    fwUseForwardLink(${ARGV})
endmacro(fwUseForward)

# Include the projects in parameter and export them.
# Compiling warnings will not be reported (because of SYSTEM).
macro(fwForwardInclude)
    target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${ARGV})
endmacro(fwForwardInclude)

macro(fwForwardLink)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ${ARGV})
endmacro(fwForwardLink)

# Include the projects in parameter but do not export them.
# Compiling warnings will not be reported (because of SYSTEM).
macro(fwInclude)
    target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${ARGV})
endmacro(fwInclude)

macro(fwLink)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${ARGV})
endmacro(fwLink)


# Defines project's linked dependencies on others projects
# example :
#     fwUse( fwCore fwData )
# WARNING : some part of this cmake file relies on this macro signature

macro(fwUse)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${ARGV})

    set(${FWPROJECT_NAME}_DEPENDENCIES)
    set(${FWPROJECT_NAME}_EXECUTABLE_DEPENDENCIES)
    set(${FWPROJECT_NAME}_LIBRARY_DEPENDENCIES)
    set(${FWPROJECT_NAME}_BUNDLE_DEPENDENCIES)

    foreach(PROJECT ${ARGV})
        list(APPEND ${FWPROJECT_NAME}_DEPENDENCIES ${PROJECT})
        list(APPEND ${FWPROJECT_NAME}_${${PROJECT}_TYPE}_DEPENDENCIES ${PROJECT})
    endforeach(PROJECT)

    set(${FWPROJECT_NAME}_DEPENDENCIES ${${FWPROJECT_NAME}_DEPENDENCIES} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_EXECUTABLE_DEPENDENCIES ${${FWPROJECT_NAME}_EXECUTABLE_DEPENDENCIES} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_BUNDLE_DEPENDENCIES ${${FWPROJECT_NAME}_BUNDLE_DEPENDENCIES} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_LIBRARY_DEPENDENCIES ${${FWPROJECT_NAME}_LIBRARY_DEPENDENCIES} PARENT_SCOPE)

endmacro(fwUse)



# Defines project's requirements on others projects
# example :
#     fwReq( ioVTK ioITK )
# WARNING : some part of this cmake file relies on this macro signature
macro(fwReq)
    add_dependencies(${FWPROJECT_NAME} ${ARGV})

    set(${FWPROJECT_NAME}_REQUIREMENTS)
    set(${FWPROJECT_NAME}_EXECUTABLE_REQUIREMENTS)
    set(${FWPROJECT_NAME}_LIBRARY_REQUIREMENTS)
    set(${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS)

    foreach(PROJECT ${ARGV})
        list(APPEND ${FWPROJECT_NAME}_REQUIREMENTS ${PROJECT})
        list(APPEND ${FWPROJECT_NAME}_${${PROJECT}_TYPE}_REQUIREMENTS ${PROJECT})
    endforeach(PROJECT)

    set(${FWPROJECT_NAME}_REQUIREMENTS ${${FWPROJECT_NAME}_REQUIREMENTS} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_EXECUTABLE_REQUIREMENTS ${${FWPROJECT_NAME}_EXECUTABLE_REQUIREMENTS} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS ${${FWPROJECT_NAME}_BUNDLE_REQUIREMENTS} PARENT_SCOPE)
    set(${FWPROJECT_NAME}_LIBRARY_REQUIREMENTS ${${FWPROJECT_NAME}_LIBRARY_REQUIREMENTS} PARENT_SCOPE)

endmacro(fwReq)


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
        endif(NOT ${DEPENDENCY}_TYPE)
    endforeach(DEPENDENCY)
    set(${RESULT} ${DEPENDENCY_LIST} PARENT_SCOPE)
endfunction(checkMissingDependencies)


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

function(findTests FWPROJECTS RESULT_VAR)
    set(RESULT "")

    foreach(PROJECT ${FWPROJECTS})
        if(${PROJECT}Test_DIR)
            list(APPEND RESULT ${PROJECT}Test)
        endif()
    endforeach()

    set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

macro(loadProperties PROPERTIES_FILE)
    set(NAME)
    set(OPTIONS)
    set(VERSION)
    set(TYPE)
    set(DEPENDENCIES)
    set(REQUIREMENTS)
    set(CPPUNITTEST_OPTIONS)

    include("${PROPERTIES_FILE}")
endmacro()


macro(fwLoadProperties)
    loadProperties("Properties.cmake")

    string( TOUPPER "${TYPE}" TYPE )
    
    if( TYPE STREQUAL "EXECUTABLE" )
        fwExec(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "LIBRARY" )
        fwLib(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "BUNDLE" )
        fwBundle(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "TEST" )
        fwCppunitTest(${NAME} "${CPPUNITTEST_OPTIONS}" "${OPTIONS}")
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



set(EXTERNAL_LIBRARIES CACHE PATH "External libraries location")
if(EXTERNAL_LIBRARIES)
    list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARIES})
    list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_LIBRARIES})

    file(GLOB LIB_CONFIGS ${EXTERNAL_LIBRARIES}/fw-*.cmake )
    foreach(LIB_CONFIG ${LIB_CONFIGS})
        message(STATUS "include : ${LIB_CONFIG}")
        include("${LIB_CONFIG}")
    endforeach(LIB_CONFIG)
endif(EXTERNAL_LIBRARIES)


set(PROJECTS_TO_BUILD CACHE STRING 
        "List of projects that will be configured for build.
        Leave empty to configure all projects"
    )

set(PROJECTS_TO_INSTALL CACHE STRING 
        "List of projects for which the installation rules will be created."
    )


file(GLOB_RECURSE PROJECTS_PROPERTIES */Properties.cmake)

set(ADDITIONAL_PROJECTS CACHE PATH 
        "Paths to the additional projects"
    )
set(ADDITIONAL_PROJECTS_SOURCE_DIR "")
if(ADDITIONAL_PROJECTS)
    foreach(ADDITIONAL_PROJECT ${ADDITIONAL_PROJECTS})
        file(GLOB_RECURSE ADDITIONAL_PROJECTS_PROPERTIES ${ADDITIONAL_PROJECT}/*/Properties.cmake)
        list(APPEND PROJECTS_PROPERTIES ${ADDITIONAL_PROJECTS_PROPERTIES})
        set(ADDITIONAL_PROJECTS_SOURCE_DIR "${ADDITIONAL_PROJECTS_SOURCE_DIR} ${ADDITIONAL_PROJECT}")
    endforeach()
endif(ADDITIONAL_PROJECTS)

foreach(PROPERTIES_FILE ${PROJECTS_PROPERTIES})
    get_filename_component(PROJECT_DIR ${PROPERTIES_FILE} PATH)

    loadProperties("${PROPERTIES_FILE}")

    if(NOT NAME)
        message(FATAL_ERROR "${PROPERTIES_FILE}: Project NAME can not be empty.")
    endif()

    list(FIND DEPENDENCIES ${NAME} DEP_LOOP)
    if(DEP_LOOP GREATER -1)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project can not be it's own dependency.")
    endif()

    list(FIND REQUIREMENTS ${NAME} REQ_LOOP)
    if(REQ_LOOP GREATER -1)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project can not be it's own requirement.")
    endif()

    if(${NAME}_DIR)
        message(FATAL_ERROR "${PROPERTIES_FILE}: A project NAME must be unique in the workspace. ${NAME} already defined there : ${${NAME}_DIR}")
    endif()

    set(${NAME}_DEPENDENCIES "${DEPENDENCIES}")
    set(${NAME}_REQUIREMENTS "${REQUIREMENTS}")
    set(${NAME}_VERSION "${VERSION}")
    set(${NAME}_DIR "${PROJECT_DIR}")

    LIST(APPEND PROJECT_LIST ${NAME})

endforeach()

# Doxygen documentation
include(${CMAKE_ROOT}/Modules/Documentation.cmake)
option(BUILD_DOCUMENTATION "Build the doxygen documentation" OFF)
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(INCLUDE_DIRS)
        foreach(PROJECT ${PROJECT_LIST})
            set(INCLUDE_DIRS "${INCLUDE_DIRS} ${${PROJECT}_DIR}/include")
        endforeach(PROJECT)
        configure_file(${FWCMAKE_RESOURCE_PATH}/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT
            "Generating API documentation with Doxygen" VERBATIM)
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCUMENTATION)

if(PROJECTS_TO_BUILD)
    set(PROJECT_LIST ${PROJECTS_TO_BUILD})
    findAllDependencies("${PROJECT_LIST}" PROJECT_LIST)
    message(STATUS "Project configuration is restricted to : ${PROJECTS_TO_BUILD}")
endif()

if(BUILD_TESTS)
    findTests("${PROJECT_LIST}" PROJECT_TESTS)
    set(PROJECT_LIST ${PROJECT_LIST};${PROJECT_TESTS})
endif()

findAllDependencies("${PROJECTS_TO_INSTALL}" PROJECTS_TO_INSTALL)

if(PROJECTS_TO_INSTALL)
    message(STATUS "Installation rules will be created for : ${PROJECTS_TO_INSTALL}")
endif()

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

foreach(PROJECT ${PROJECT_LIST})
    addProject(${PROJECT})
endforeach()

# Eclipse project
option(ECLIPSE_PROJECT "Generate eclipse project" OFF)
if(ECLIPSE_PROJECT)
    include(${FWCMAKE_RESOURCE_PATH}eclipse/eclipse_generator.cmake)
    eclipseGenerator(${PROJECT_LIST})
endif(ECLIPSE_PROJECT)

# command new service
add_custom_target(newService
        COMMAND ${CMAKE_COMMAND} -DSOURCEDIR:VAR=${CMAKE_CURRENT_SOURCE_DIR} 
                                 -DADDITIONALDIR="${ADDITIONAL_PROJECTS}"
                                 -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/cmake_newService.cmake)

# command new bundle
add_custom_target(newBundle
        COMMAND ${CMAKE_COMMAND} -DSOURCEDIR:VAR=${CMAKE_CURRENT_SOURCE_DIR}
                                 -DADDITIONALDIR="${ADDITIONAL_PROJECTS}" 
                                 -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/cmake_newBundle.cmake)


