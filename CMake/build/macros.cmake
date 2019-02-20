# Define some paths whether we are building Sight or using it
if(FW_BUILD_EXTERNAL)
    set(FWCMAKE_BUILD_FILES_DIR ${CMAKE_CURRENT_LIST_DIR}/build)
    set(FWCMAKE_INSTALL_FILES_DIR ${CMAKE_CURRENT_LIST_DIR}/install)
    set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_LIST_DIR})
else()
    set(FWCMAKE_BUILD_FILES_DIR "${CMAKE_SOURCE_DIR}/CMake/build/")
    set(FWCMAKE_INSTALL_FILES_DIR "${CMAKE_SOURCE_DIR}/CMake/install/")
    # FWCMAKE_RESOURCE_PATH already set in main CMakeLists.txt
endif()

# Define the path 'FW_EXTERNAL_LIBRARIES_DIR' used to find external libraries required by our applications
macro(setExternalLibrariesDir)
    if(USE_CONAN)
        if(FW_BUILD_EXTERNAL)
            if(WIN32)
                set(FW_EXTERNAL_LIBRARIES_DIR "${Sight_BINARY_DIR}")
            else()
                set(FW_EXTERNAL_LIBRARIES_DIR "${Sight_LIBRARY_DIR}/..")
            endif()
        else()
            if(WIN32)
                set(FW_EXTERNAL_LIBRARIES_DIR "${CONAN_BIN_DIRS}")
            else()
                set(FW_EXTERNAL_LIBRARIES_DIR "${CONAN_LIB_DIRS}")
            endif()
        endif()
    else()
        if(WIN32)
            set(FW_EXTERNAL_LIBRARIES_DIR "${EXTERNAL_LIBRARIES}/bin")
        else()
            set(FW_EXTERNAL_LIBRARIES_DIR "${EXTERNAL_LIBRARIES}/lib")
        endif()
    endif()
endmacro()

include(${FWCMAKE_INSTALL_FILES_DIR}/helper.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/plugin_config.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/profile_config.cmake)
include(${FWCMAKE_INSTALL_FILES_DIR}/generic_install.cmake)

file(REMOVE "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake")

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

macro(configure_header_file FWPROJECT_NAME FILENAME)

    set(HEADER_FILE_DESTINATION "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/${FILENAME}")
    configure_file(
        "${FWCMAKE_BUILD_FILES_DIR}/${FILENAME}.in"
        ${HEADER_FILE_DESTINATION}
        IMMEDIATE @ONLY)

    if(BUILD_SDK)
        install(FILES ${HEADER_FILE_DESTINATION}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${FWPROJECT_NAME})
    endif()
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
    set (${FWPROJECT_NAME}_DIR       ${CMAKE_CURRENT_SOURCE_DIR})
    set (${FWPROJECT_NAME}_DIR       ${${FWPROJECT_NAME}_DIR}  PARENT_SCOPE)
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

    target_include_directories(${FWPROJECT_NAME} PUBLIC ${${FWPROJECT_NAME}_INCLUDE_DIR})

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
        endif()
    endif()

    # Configure launcher script
    if(UNIX)
        string(TOLOWER ${FWPROJECT_NAME}.sh ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE ${${FWPROJECT_NAME}_FULLNAME})

        # Use the right path separator on unix
        string(REPLACE ";" ":" FW_EXTERNAL_LIBRARIES_DIRS "${FW_EXTERNAL_LIBRARIES_DIR}")

        # Build the shell script from template_exe.sh.in
        configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template_exe.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)

        # Cleanup
        unset(FW_EXTERNAL_LIBRARIES_DIRS)

        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    elseif(WIN32)
        string(TOLOWER ${FWPROJECT_NAME}.bat ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE ${FWPROJECT_NAME})

        configure_file(${FWCMAKE_RESOURCE_PATH}/build/windows/template_exe.bat.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)
        unset(FW_EXTERNAL_LIBRARIES_DIRS)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()

    if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
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
    set(oneValueArgs)
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

    configureProject( ${FWPROJECT_NAME} 0.0 )

    target_include_directories(${FWPROJECT_NAME} PUBLIC ${${FWPROJECT_NAME}_INCLUDE_DIR})

    if(EXISTS "${PRJ_SOURCE_DIR}/tu/rc")
        if(EXISTS "${PRJ_SOURCE_DIR}/tu/rc/profile.xml")
            target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DBUNDLE_TEST_PROFILE=\"${FWBUNDLE_RC_PREFIX}/${TU_NAME}/profile.xml\")
        endif()
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${TU_NAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/tu/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${FWBUNDLE_RC_PREFIX}/${TU_NAME}" )
        endif()
    endif()

    # Configure launcher script
    if(UNIX)
        string(TOLOWER ${FWPROJECT_NAME}.sh ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE ${${FWPROJECT_NAME}_FULLNAME})

        # Use the right path separator on unix
        string(REPLACE ";" ":" FW_EXTERNAL_LIBRARIES_DIRS "${FW_EXTERNAL_LIBRARIES_DIR}")

        # Build the shell script from template_test.sh.in
        configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template_test.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)

        # Cleanup
        unset(FW_EXTERNAL_LIBRARIES_DIRS)

        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    elseif(WIN32)
        string(TOLOWER ${FWPROJECT_NAME}.bat ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE ${FWPROJECT_NAME})

        configure_file(${FWCMAKE_RESOURCE_PATH}/build/windows/template_exe.bat.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)
        unset(FW_EXTERNAL_LIBRARIES_DIRS)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()

    if(TESTS_XML_OUTPUT)
        add_test(NAME ${FWPROJECT_NAME} COMMAND "${${FWPROJECT_NAME}_SCRIPT} --xml" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        set_tests_properties(${FWPROJECT_NAME} PROPERTIES TIMEOUT 240)
    else()
        add_test(NAME ${FWPROJECT_NAME} COMMAND "${${FWPROJECT_NAME}_SCRIPT}" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
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

    add_library(${FWPROJECT_NAME} SHARED ${ARGN}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${${FWPROJECT_NAME}_RC_FILES}
        ${${FWPROJECT_NAME}_CMAKE_FILES}
        ${${FWPROJECT_NAME}_PCH_LIB})

    configureProject( ${FWPROJECT_NAME} ${PROJECT_VERSION} )

    # Set interface properties
    set_target_properties(${FWPROJECT_NAME} PROPERTIES INTERFACE_${FWPROJECT_NAME}_MAJOR_VERSION ${API_VERSION})
    set_target_properties(${FWPROJECT_NAME} PROPERTIES COMPATIBLE_INTERFACE_STRING ${FWPROJECT_NAME}_MAJOR_VERSION)

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
            createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
        endif()

    endif()

    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    # create the config.hpp for the current library
    configure_header_file(${FWPROJECT_NAME} "config.hpp")

    target_include_directories(${FWPROJECT_NAME} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/>
    )

    set(${FWPROJECT_NAME}_INCLUDE_INSTALL_DIR ${FW_INSTALL_PATH_SUFFIX}/${FWPROJECT_NAME} PARENT_SCOPE)

    if(BUILD_SDK)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/${FWPROJECT_NAME}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX})

        install(
            TARGETS ${FWPROJECT_NAME} EXPORT Sight-${FWPROJECT_NAME}Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${FW_INSTALL_PATH_SUFFIX}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${FW_INSTALL_PATH_SUFFIX}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${FW_INSTALL_PATH_SUFFIX}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}
        )

        # Add all targets to the build-tree export set
        export( EXPORT Sight-${FWPROJECT_NAME}Targets
                FILE "${CMAKE_BINARY_DIR}/cmake/Sight-${FWPROJECT_NAME}Targets.cmake"
                NAMESPACE Sight::)

        # Install the Sight-ProjectTargets.cmake
        install(EXPORT Sight-${FWPROJECT_NAME}Targets
                FILE
                  Sight-${FWPROJECT_NAME}Targets.cmake
                NAMESPACE
                  Sight::
                DESTINATION
                  ${FWCONFIG_PACKAGE_LOCATION}
        )

        list(APPEND FW_COMPONENTS ${FWPROJECT_NAME} )
        set(FW_COMPONENTS ${FW_COMPONENTS} PARENT_SCOPE)

        # Add Sight targets depepndencies
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
            configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake"
                            "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                            COPYONLY IMMEDIATE)
        else()
            # Create empty file
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake" "")
        endif()

        set(FW_INSTALL_DEPENDENCIES ${${FWPROJECT_NAME}_DEPENDENCIES})
        list(REMOVE_ITEM FW_INSTALL_DEPENDENCIES ${FWPROJECT_NAME})
        if(FW_INSTALL_DEPENDENCIES)
            file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                "find_package(Sight COMPONENTS ${FW_INSTALL_DEPENDENCIES} REQUIRED)")
        endif()

        # Install the Sight-ProjectDependencies.cmake
        if( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake")
            install(FILES
                      "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                    RENAME
                        Sight-${FWPROJECT_NAME}Dependencies.cmake
                    DESTINATION
                      ${FWCONFIG_PACKAGE_LOCATION}
            )
        endif()
    else()
        if(${FWPROJECT_NAME}_INSTALL)
            install(
                TARGETS ${FWPROJECT_NAME}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${FW_INSTALL_PATH_SUFFIX}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${FW_INSTALL_PATH_SUFFIX}
                OPTIONAL NAMELINK_SKIP
                )
        endif()
    endif()


    # Adds project into folder lib
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "lib")

    if(ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        # create the spyLogLevel.hpp for the current library
        configure_header_file(${FWPROJECT_NAME} "spyLogLevel.hpp")

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

    set(BUNDLE_DIR "${CMAKE_BINARY_DIR}/${FWBUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

    if(EXISTS "${PRJ_SOURCE_DIR}/src")

        add_library(${FWPROJECT_NAME} SHARED ${ARGN}
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

        set_target_properties(${FWPROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUNDLE_DIR})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUNDLE_DIR})
        # Fixed path for multi-config builds (e.g. msvc)
        foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
            string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
            set_target_properties(${FWPROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
            set_target_properties(${FWPROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
            set_target_properties(${FWPROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BUNDLE_DIR})
        endforeach()

        if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
            qt_plugins_setup(${FWPROJECT_NAME}) # search and setup qt plugins for each bundles
            install(
                TARGETS ${FWPROJECT_NAME}
                RUNTIME DESTINATION ${FWBUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}
                LIBRARY DESTINATION ${FWBUNDLE_LIB_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}
                OPTIONAL NAMELINK_SKIP
                )
        endif()

        # create the config.hpp for the current library
        configure_file(
            "${FWCMAKE_BUILD_FILES_DIR}/config.hpp.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/${FWPROJECT_NAME}/config.hpp"
            IMMEDIATE @ONLY)

        target_include_directories(${FWPROJECT_NAME} PUBLIC ${${FWPROJECT_NAME}_INCLUDE_DIR})
        target_include_directories(${FWPROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/include/")

        if(ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
            # create the spyLogLevel.hpp for the current library
            configure_file(
                "${FWCMAKE_BUILD_FILES_DIR}/spyLogLevel.hpp.in"
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
            set(PROFILE "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/profile.xml")
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

            set(LAUNCHER "fwlauncher-${fwlauncher_VERSION}")
            set(PROFILE_PATH "${${FWPROJECT_NAME}_FULLNAME}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}")
            else()
                set(LAUNCHER_PATH "$me")
            endif()

            # Configure launcher script
            # Replace all ';' path separator to unix style path separator ':'
            string(REPLACE ";" ":" FW_EXTERNAL_LIBRARIES_DIRS "${FW_EXTERNAL_LIBRARIES_DIR}")
            configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
            unset(FW_EXTERNAL_LIBRARIES_DIRS)

            file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        elseif(WIN32)
            # Install shortcut
            string(TOLOWER ${FWPROJECT_NAME} APP_NAME)

            set(LAUNCHER "fwlauncher.exe")
            set(PROFILE_PATH "${${FWPROJECT_NAME}_FULLNAME}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}\\${LAUNCHER}")
            else()
                set(LAUNCHER_PATH "%BINDIR%\\${LAUNCHER}")
            endif()

            file(TO_NATIVE_PATH "${PROFILE_PATH}" PROFILE_PATH)

            configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/template.bat.in ${CMAKE_BINARY_DIR}/bin/${APP_NAME}.bat @ONLY)
        endif()
    else()
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "bundle")
    endif()

    set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )
    endif()

    if(${FWPROJECT_NAME}_INSTALL OR BUILD_SDK)
        createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}" )
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
    foreach(DEPENDENCY ${ARGV})
        if(NOT ${DEPENDENCY}_EXTERNAL)
            add_dependencies(${FWPROJECT_NAME} ${DEPENDENCY})
        endif()
    endforeach()

    set(${FWPROJECT_NAME}_REQUIREMENTS)

    foreach(PROJECT ${ARGV})
        list(APPEND ${FWPROJECT_NAME}_REQUIREMENTS ${PROJECT})
        list(APPEND ${FWPROJECT_NAME}_${${PROJECT}_TYPE}_REQUIREMENTS ${PROJECT})
    endforeach()

    set(${FWPROJECT_NAME}_REQUIREMENTS ${${FWPROJECT_NAME}_REQUIREMENTS} PARENT_SCOPE)

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

function(findProjectSubdirectory PROJECT_DIR REPOSITORY_DIRECTORIES RESULT_DIR)
    # Search for the project in all the repositories (sight + additional repos).
    foreach(REPO_DIR ${REPOSITORY_DIRECTORIES})
        # Try to strip the repo's directory from the project's directory.
        string(REPLACE "${REPO_DIR}/" "" RESULT ${PROJECT_DIR})
        if(NOT ${RESULT} STREQUAL ${PROJECT_DIR})
            # If it succeeded that means that the project repo was found. We can stop now.
            set(${RESULT_DIR} ${RESULT} PARENT_SCOPE)
            break()
        endif()
    endforeach()
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
    unset(PLUGINS)
    unset(CONAN_DEPS)

    include("${PROPERTIES_FILE}")
endmacro()


macro(fwLoadProperties)
    loadProperties("Properties.cmake")

    if(FW_BUILD_EXTERNAL)
        set(${NAME}_DEPENDENCIES "${DEPENDENCIES}")
        set(${NAME}_REQUIREMENTS "${REQUIREMENTS}")
    endif()

    string( TOUPPER "${TYPE}" TYPE )

    if(PLUGINS)
        set(${NAME}_PLUGINS ${PLUGINS})
        set(${NAME}_PLUGINS ${PLUGINS} PARENT_SCOPE)
    endif()

    fwDefineDependencies( ${NAME} ${DEPENDENCIES} )

    if( TYPE STREQUAL "EXECUTABLE" )
        fwExec(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "LIBRARY" )
        fwLib(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "BUNDLE" )
        set(${NAME}_TYPE "BUNDLE")
        fwBundle(${NAME} ${VERSION} ${OPTIONS})
    elseif( TYPE STREQUAL "TEST" )
        fwCppunitTest(${NAME} "${OPTIONS}")
    elseif( TYPE STREQUAL "APP" )
        set(${NAME}_TYPE "APP")
        fwBundle(${NAME} ${VERSION} ${OPTIONS})
        if(NOT EXISTS "${${NAME}_DIR}/rc/profile.xml" )
            set(PROJECT ${NAME})
            profile_setup(${PROJECT})
        endif()
    endif()

    if(DEPENDENCIES)
        fwUse( ${DEPENDENCIES} )
    endif()
    if(REQUIREMENTS)
        fwReq( ${REQUIREMENTS} )
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
        include("Dependencies.cmake")
    endif()

    # Generate batch script to ease the set of PATH in order to launch a Sight application on Windows.
    if(WIN32 AND USE_CONAN)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/setpath.bat.in ${CMAKE_BINARY_DIR}/bin/setpath.bat @ONLY)
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

        if(BUILD_SDK)
            # Store requirements for the SDK
            file(APPEND "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
                "set(${PROJECT}_EXTERNAL 1)\n"
                "set(${PROJECT}_REQUIREMENTS ${${PROJECT}_REQUIREMENTS})\n"
                "set(${PROJECT}_DEPENDENCIES ${${PROJECT}_DEPENDENCIES})\n"
                "set(${PROJECT}_VERSION ${${PROJECT}_VERSION})\n"
                "set(${PROJECT}_TYPE ${${PROJECT}_TYPE})\n")
            if(${PROJECT}_START)
                file(APPEND "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
                    "set(${PROJECT}_START ${${PROJECT}_START})\n")
            endif()
            if(${PROJECT}_PLUGINS)
                file(APPEND "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
                    "set(${PROJECT}_PLUGINS ${${PROJECT}_PLUGINS})\n")
            endif()
            if(${PROJECT}_CONAN_DEPS)
                file(APPEND "${CMAKE_BINARY_DIR}/cmake/SightRequirements.cmake"
                    "set(${PROJECT}_CONAN_DEPS ${${PROJECT}_CONAN_DEPS})\n")
            endif()
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
