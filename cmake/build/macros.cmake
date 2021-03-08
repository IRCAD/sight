set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Define some paths whether we are building Sight or using it
if(FW_BUILD_EXTERNAL)
    set(FWCMAKE_BUILD_FILES_DIR ${CMAKE_CURRENT_LIST_DIR}/build)
    set(FWCMAKE_INSTALL_FILES_DIR ${CMAKE_CURRENT_LIST_DIR}/install)
    set(FWCMAKE_RESOURCE_PATH ${CMAKE_CURRENT_LIST_DIR})
else()
    set(FWCMAKE_BUILD_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/")
    set(FWCMAKE_INSTALL_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/")
    # FWCMAKE_RESOURCE_PATH already set in main CMakeLists.txt
endif()

# Define the path 'FW_SIGHT_EXTERNAL_LIBRARIES_DIR' used to find external libraries required by our applications
macro(setExternalLibrariesDir)
    if(VCPKG_TARGET_TRIPLET)
        if(FW_BUILD_EXTERNAL)
            if(WIN32)
                set(FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${Sight_BINARY_DIR}")
            else()
                set(FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${Sight_LIBRARY_DIR}/..")
            endif()
        endif()
    endif()
endmacro()

include(${FWCMAKE_INSTALL_FILES_DIR}/helper.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/plugin_config.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/profile_config.cmake)
include(${FWCMAKE_INSTALL_FILES_DIR}/generic_install.cmake)
if(NOT FW_BUILD_EXTERNAL)
    include(${FWCMAKE_INSTALL_FILES_DIR}/get_git_rev.cmake)
endif()

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

function(get_header_file_install_destination)
    # Paths for config files are:
    # activities -> activity/theme/project/
    # apps -> project
    # examples -> project
    # libs -> theme/project/ except for theme=core project
    # modules -> modules/theme/project/  except for theme=core modules/project
    # tutorials -> project
    file(RELATIVE_PATH CURRENT_SOURCE_DIR_REL ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "/" ";" CURRENT_SOURCE_DIRS ${CURRENT_SOURCE_DIR_REL})

    list(GET CURRENT_SOURCE_DIRS 0 ROOT)
    list(LENGTH CURRENT_SOURCE_DIRS CURRENT_SOURCE_DIRS_LEN)
    if(CURRENT_SOURCE_DIRS_LEN LESS 3)
        list(GET CURRENT_SOURCE_DIRS 1 PROJECT)
        set(PROJECT_PATH "${PROJECT}")
    else()
        list(GET CURRENT_SOURCE_DIRS 1 THEME)
        list(GET CURRENT_SOURCE_DIRS 2 PROJECT)
        string(REPLACE "core" "." THEME ${THEME})
        set(PROJECT_PATH "${THEME}/${PROJECT}")
    endif()
    
    if("${ROOT}" STREQUAL "libs")
        set(HEADER_FILE_DESTINATION_REL "${PROJECT_PATH}" PARENT_SCOPE)
    else()
        if("${ROOT}" STREQUAL "modules" OR "${ROOT}" STREQUAL "activities" )
            set(HEADER_FILE_DESTINATION_REL "${ROOT}/${PROJECT_PATH}" PARENT_SCOPE)
        else()
            set(HEADER_FILE_DESTINATION_REL "${PROJECT}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

macro(configure_header_file FWPROJECT_NAME FILENAME HEADER_FILE_DESTINATION_REL)
    set(HEADER_FILE_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER_FILE_DESTINATION_REL}")
    configure_file(
        "${FWCMAKE_BUILD_FILES_DIR}/${FILENAME}.in"
        ${HEADER_FILE_DESTINATION}/${FILENAME}
        IMMEDIATE @ONLY)

    install(FILES ${HEADER_FILE_DESTINATION}/${FILENAME}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${HEADER_FILE_DESTINATION_REL})
endmacro()

macro(initProject PRJ_NAME PRJ_TYPE)
    if(CREATE_SUBPROJECTS)
        project( ${PRJ_NAME} )
    endif()
    set(FWPROJECT_NAME ${PRJ_NAME})
    set(PRJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set(${FWPROJECT_NAME}_HEADERS)
    set(${FWPROJECT_NAME}_SOURCES)

    set(SUBDIRS ${ARGN})
    list(LENGTH SUBDIRS NB_SUBDIRS)
    if(NB_SUBDIRS EQUAL 0)
        list(APPEND SUBDIRS ${PRJ_SOURCE_DIR})
    endif()

    foreach(SUBDIR ${SUBDIRS})
        file(GLOB_RECURSE HEADERS "${SUBDIR}/*.hpp" "${SUBDIR}/*.h" "${SUBDIR}/*.hxx")
        file(GLOB_RECURSE SOURCES "${SUBDIR}/*.cpp" "${SUBDIR}/*.c" "${SUBDIR}/*.cxx")
        
        if (NOT "${PRJ_TYPE}" STREQUAL "TEST")
            list(FILTER SOURCES EXCLUDE REGEX "/test/api")
            list(FILTER SOURCES EXCLUDE REGEX "/test/detail")
            list(FILTER SOURCES EXCLUDE REGEX "/test/tu")
            list(FILTER HEADERS EXCLUDE REGEX "/test/api")
            list(FILTER HEADERS EXCLUDE REGEX "/test/detail")
            list(FILTER HEADERS EXCLUDE REGEX "/test/tu")
        endif()
        
        list(APPEND ${FWPROJECT_NAME}_HEADERS ${HEADERS})
        list(APPEND ${FWPROJECT_NAME}_SOURCES ${SOURCES})
    endforeach()

    set (${FWPROJECT_NAME}_DIR       ${CMAKE_CURRENT_SOURCE_DIR})
    set (${FWPROJECT_NAME}_DIR       ${${FWPROJECT_NAME}_DIR}  PARENT_SCOPE)
    set (${FWPROJECT_NAME}_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set (${FWPROJECT_NAME}_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

    set (${FWPROJECT_NAME}_HEADERS ${${FWPROJECT_NAME}_HEADERS} PARENT_SCOPE)
    set (${FWPROJECT_NAME}_SOURCES ${${FWPROJECT_NAME}_SOURCES} PARENT_SCOPE)

    file(GLOB_RECURSE ${FWPROJECT_NAME}_RC_FILES "${PRJ_SOURCE_DIR}/rc/*" "${PRJ_SOURCE_DIR}/tu/rc/*")
    set(${FWPROJECT_NAME}_RC_FILES ${${FWPROJECT_NAME}_RC_FILES} PARENT_SCOPE)
    set_source_files_properties(${${FWPROJECT_NAME}_RC_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    file(GLOB ${FWPROJECT_NAME}_CMAKE_FILES "${PRJ_SOURCE_DIR}/*.txt" "${PRJ_SOURCE_DIR}/*.cmake")
    set(${FWPROJECT_NAME}_CMAKE_FILES ${${FWPROJECT_NAME}_CMAKE_FILES} PARENT_SCOPE)
    set_source_files_properties(${${FWPROJECT_NAME}_CMAKE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    groupMaker(${FWPROJECT_NAME})
endmacro()


macro(configureProject FWPROJECT_NAME)
    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    if(${FWPROJECT_NAME}_OBJECT_LIB)
        set(TARGET_NAME ${FWPROJECT_NAME_OBJECT_LIB})
    else()
        set(TARGET_NAME ${FWPROJECT_NAME})
    endif()

    set_target_properties(${FWPROJECT_NAME} PROPERTIES VERSION ${SIGHT_VERSION} SOVERSION ${SIGHT_API_VERSION})

    target_compile_definitions(${TARGET_NAME} PRIVATE ${PROJECT_NAME_UPCASE}_EXPORTS)

    # Get CMake target type (not Sight one)
    get_target_property(TARGET_TYPE ${FWPROJECT_NAME} TYPE)
    
    if (NOT ${TARGET_TYPE} MATCHES "EXECUTABLE")
        set(${FWPROJECT_NAME}_LIBRARY "$<TARGET_FILE:${FWPROJECT_NAME}>")
        set(${FWPROJECT_NAME}_LIBRARY ${${FWPROJECT_NAME}_LIBRARY} PARENT_SCOPE)
    endif()

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


macro(fwExec FWPROJECT_NAME)
    set(options CONSOLE)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(FWEXEC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

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

    set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "EXECUTABLE")

    # On linux add ".bin" suffix, to distinguish executable (.bin) from launcher script(.sh).
    if(UNIX)
        set_target_properties( ${FWPROJECT_NAME}
            PROPERTIES
            SUFFIX ".bin"
        )
    endif()

    configureProject( ${FWPROJECT_NAME} )

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}" )
    endif()

    # Configure launcher script
    if(UNIX)

        string(TOLOWER ${FWPROJECT_NAME} ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE "${FWPROJECT_NAME}.bin")

        # Use the right path separator on unix
        if(SIGHT_EXTERNAL_LIBRARIES)
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${SIGHT_EXTERNAL_LIBRARIES}/lib")
        else()
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
        endif()

        # Build the shell script from template_exe.sh.in
        configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template_exe.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)

        # Cleanup
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIRS)

        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    elseif(WIN32)
        string(TOLOWER ${FWPROJECT_NAME}.bat ${FWPROJECT_NAME}_SCRIPT)
        set(PROJECT_EXECUTABLE ${FWPROJECT_NAME})

        configure_file(${FWCMAKE_RESOURCE_PATH}/build/windows/template_exe.bat.in ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} @ONLY)
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIRS)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${${FWPROJECT_NAME}_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

        if(MSVC_IDE)
            set(LAUNCHER "${CMAKE_BINARY_DIR}/bin/${FWPROJECT_NAME}")
            set(PROFILE "")
            set(WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
            if(CMAKE_CL_64)
                set(PLATFORM "x64")
            else()
                set(PLATFORM "Win32")
            endif()
            configure_file(
                "${CMAKE_SOURCE_DIR}/cmake/build/project.vcxproj.user.in"
                "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/${FWPROJECT_NAME}.vcxproj.user"
                IMMEDIATE @ONLY)
        endif()

    endif()

    install(
        TARGETS ${FWPROJECT_NAME}
        RUNTIME DESTINATION bin
        OPTIONAL
        )

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
        "${CMAKE_CURRENT_BINARY_DIR}/src/cppunit_main.cpp"
        IMMEDIATE @ONLY)

    string(REGEX REPLACE "Test$" "" DIRNAME "${FWPROJECT_NAME}")
    set(TU_NAME "tu_exec_${DIRNAME}")

    add_executable(${FWPROJECT_NAME}
        ${fwCppunitTest_UNPARSED_ARGUMENTS}
        ${${FWPROJECT_NAME}_HEADERS}
        ${${FWPROJECT_NAME}_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/src/cppunit_main.cpp
        ${${FWPROJECT_NAME}_RC_FILES}
        ${${FWPROJECT_NAME}_CMAKE_FILES})

    # Do it here because add ".bin" suffix change the ${FWPROJECT_NAME} (!!!)
    if(UNIX)
        set(PROJECT_EXECUTABLE "${FWPROJECT_NAME}.bin")
        string(TOLOWER "${FWPROJECT_NAME}" SIGHT_TEST_SCRIPT)
        
        # On linux add ".bin" suffix, to distinguish executable (.bin) from launcher script(.sh).
        set_target_properties( ${FWPROJECT_NAME}
            PROPERTIES
            SUFFIX ".bin"
        )
    else()
        set(PROJECT_EXECUTABLE "${FWPROJECT_NAME}")
        string(TOLOWER "${FWPROJECT_NAME}.bat" SIGHT_TEST_SCRIPT)
    endif()

    set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "TEST")

    configureProject( ${FWPROJECT_NAME} )

    if(EXISTS "${PRJ_SOURCE_DIR}/tu/rc")
        set(TEST_RC_DIR "${PRJ_SOURCE_DIR}/tu/rc")
    elseif(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(TEST_RC_DIR "${PRJ_SOURCE_DIR}/rc")
    endif()
    if(TEST_RC_DIR)
        if(EXISTS "${TEST_RC_DIR}/profile.xml")
            target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DMODULE_TEST_PROFILE=\"${SIGHT_MODULE_RC_PREFIX}/${TU_NAME}/profile.xml\")
        endif()
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${TU_NAME}")

        createResourcesTarget( ${FWPROJECT_NAME}_rc "${TEST_RC_DIR}" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )
    endif()

    # Configure launcher script
    if(UNIX)
        # Use the right path separator on unix
        if(SIGHT_EXTERNAL_LIBRARIES)
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${SIGHT_EXTERNAL_LIBRARIES}/lib")
        else()
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
        endif()

        # Build the shell script from template_test.sh.in
        configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template_test.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT} @ONLY)

        # Cleanup
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIRS)
    else()
        # Build the bat script from template_exe.bat.in
        configure_file(${FWCMAKE_RESOURCE_PATH}/build/windows/template_exe.bat.in ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT} @ONLY)
    endif()

    # Copy launcher script
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/bin/
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    # Set test command
    if(TESTS_XML_OUTPUT)
        add_test(NAME "${SIGHT_TEST_SCRIPT}" COMMAND "${CMAKE_BINARY_DIR}/bin/${SIGHT_TEST_SCRIPT} --xml" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        set_tests_properties(${SIGHT_TEST_SCRIPT} PROPERTIES TIMEOUT 240)
    else()
        add_test(NAME "${SIGHT_TEST_SCRIPT}" COMMAND "${CMAKE_BINARY_DIR}/bin/${SIGHT_TEST_SCRIPT}" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    endif()

    # Cleanup
    unset(SIGHT_TEST_SCRIPT)

    # Adds project into folder test
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "test")

    if(MSVC_IDE)
        # create the launch config for the current test
        set(LAUNCHER "${CMAKE_BINARY_DIR}/bin/${FWPROJECT_NAME}")
        set(PROFILE "")
        set(WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
        include(${FWCMAKE_RESOURCE_PATH}/install/win_install.cmake)
        if(CMAKE_CL_64)
            set(PLATFORM "x64")
        else()
            set(PLATFORM "Win32")
        endif()
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/build/project.vcxproj.user.in"
            "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/${FWPROJECT_NAME}.vcxproj.user"
            IMMEDIATE @ONLY)
    endif()
endmacro()

macro(fwLib FWPROJECT_NAME OBJECT_LIBRARY)

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header_cpp(${FWPROJECT_NAME})
        endif()
        set(${FWPROJECT_NAME}_PCH_LIB $<TARGET_OBJECTS:${${FWPROJECT_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    if(${OBJECT_LIBRARY})
        set(FWPROJECT_NAME_OBJECT_LIB ${FWPROJECT_NAME}_obj)

        add_library(${FWPROJECT_NAME_OBJECT_LIB} OBJECT
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            ${${FWPROJECT_NAME}_PCH_LIB})

        add_library(${FWPROJECT_NAME} SHARED $<TARGET_OBJECTS:${FWPROJECT_NAME_OBJECT_LIB}> ${${FWPROJECT_NAME}_PCH_LIB})

        target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/core/>
        )
        target_include_directories(${FWPROJECT_NAME} PUBLIC
            $<INSTALL_INTERFACE:include>
        )
        target_link_libraries(${FWPROJECT_NAME} PUBLIC ${FWPROJECT_NAME_OBJECT_LIB})
    else()
        add_library(${FWPROJECT_NAME} SHARED
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            ${${FWPROJECT_NAME}_PCH_LIB})
        target_include_directories(${FWPROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/core/>
            $<INSTALL_INTERFACE:include>
        )
    endif()

    set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "LIBRARY")

    configureProject( ${FWPROJECT_NAME} )

    # Set interface properties
    set_target_properties(${FWPROJECT_NAME} PROPERTIES INTERFACE_${FWPROJECT_NAME}_MAJOR_VERSION ${SIGHT_API_VERSION})
    set_target_properties(${FWPROJECT_NAME} PROPERTIES COMPATIBLE_INTERFACE_STRING ${FWPROJECT_NAME}_MAJOR_VERSION)

    set_target_properties(${FWPROJECT_NAME} PROPERTIES OUTPUT_NAME ${SIGHT_REPOSITORY}_${FWPROJECT_NAME})

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )

        createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}" )
    endif()

    # create the config.hpp for the current library
    get_header_file_install_destination()
    configure_header_file(${FWPROJECT_NAME} "config.hpp" "${HEADER_FILE_DESTINATION_REL}")

    set(${FWPROJECT_NAME}_INCLUDE_INSTALL_DIR ${FW_INSTALL_PATH_SUFFIX}/${FWPROJECT_NAME} PARENT_SCOPE)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${HEADER_FILE_DESTINATION_REL}
            FILES_MATCHING PATTERN "*.h"
                            PATTERN "*.hpp"
                            PATTERN "*.hxx"
                            PATTERN "test/*" EXCLUDE)
    set(TARGETS_TO_EXPORT ${FWPROJECT_NAME})

    if(${OBJECT_LIBRARY})
        set(TARGETS_TO_EXPORT ${FWPROJECT_NAME} ${FWPROJECT_NAME_OBJECT_LIB})
    endif()
    install(
        TARGETS ${TARGETS_TO_EXPORT} 
        EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}
    )

    if(WIN32)
        install(
            FILES $<TARGET_PDB_FILE:${FWPROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL
        )
    endif()

    # Add all targets to the build-tree export set
    export( EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
            FILE "${CMAKE_BINARY_DIR}/cmake/${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets.cmake"
            NAMESPACE ${SIGHT_REPOSITORY}::)

    # Install sight_Project_Targets.cmake
    install(EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
            FILE
                ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets.cmake
            NAMESPACE
                ${SIGHT_REPOSITORY}::
            DESTINATION
                ${FWCONFIG_PACKAGE_LOCATION}
    )
    get_property(SIGHT_COMPONENTS GLOBAL PROPERTY SIGHT_COMPONENTS)
    set_property(GLOBAL PROPERTY SIGHT_COMPONENTS ${SIGHT_COMPONENTS};${FWPROJECT_NAME} )

    # Add Sight targets dependencies
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
        configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake"
                        "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                        COPYONLY IMMEDIATE)
    else()
        # Create empty file
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake" "")
    endif()

    # Install the sight_project_Dependencies.cmake
    install(FILES
                "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
            RENAME
                ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Dependencies.cmake
            DESTINATION
                ${FWCONFIG_PACKAGE_LOCATION}
    )

    # Adds project into folder lib
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "lib")

    if(SIGHT_ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        
        if(${OBJECT_LIBRARY})
            set(TARGET_NAME ${FWPROJECT_NAME_OBJECT_LIB})
        else()
            set(TARGET_NAME ${FWPROJECT_NAME})
        endif()
        if("${${FWPROJECT_NAME}_PCH_TARGET}" STREQUAL "${FWPROJECT_NAME}")
            add_precompiled_header(${TARGET_NAME} pch.hpp)
            if(SIGHT_VERBOSE_PCH)
                message(STATUS "Use custom precompiled header")
            endif()
        else()
            use_precompiled_header(${TARGET_NAME} ${${FWPROJECT_NAME}_PCH_TARGET})
            if(SIGHT_VERBOSE_PCH)
                message(STATUS "Use ${${FWPROJECT_NAME}_PCH_TARGET} precompiled header")
            endif()
        endif()
    endif()

endmacro()

macro(fwModule FWPROJECT_NAME TARGET_TYPE)

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header_cpp(${FWPROJECT_NAME})
        endif()
        set(${FWPROJECT_NAME}_PCH_LIB $<TARGET_OBJECTS:${${FWPROJECT_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    set(MODULE_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_LIB_PREFIX}/${FWPROJECT_NAME}")
    
    if( ${FWPROJECT_NAME}_SOURCES )

        add_library(${FWPROJECT_NAME} SHARED ${ARGN}
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            ${${FWPROJECT_NAME}_PCH_LIB}
        )

        # create the custom command that may generate the plugin.xml and the registerServices.cpp file
        if(EXISTS "${${FWPROJECT_NAME}_DIR}/rc/plugin.xml" )
            target_sources( ${FWPROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/registerServices.cpp" )
            set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/registerServices.cpp" PROPERTIES
                GENERATED TRUE
                SKIP_AUTOMOC ON)

            plugin_setup("${FWPROJECT_NAME}")
        endif()

        configureProject( ${FWPROJECT_NAME} )

        set_target_properties(${FWPROJECT_NAME} PROPERTIES INTERFACE_${FWPROJECT_NAME}_MAJOR_VERSION ${SIGHT_API_VERSION})
        set_target_properties(${FWPROJECT_NAME} PROPERTIES COMPATIBLE_INTERFACE_STRING ${FWPROJECT_NAME}_MAJOR_VERSION)
    
        # create the config.hpp for the current module
        get_header_file_install_destination()
        configure_header_file(${FWPROJECT_NAME} "config.hpp" "${HEADER_FILE_DESTINATION_REL}")

        # Allows include of type <ui/config.hpp>
        target_include_directories(${FWPROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
        # Allows include of all folders in libs, i.e. <ui/..> <io/..> ...
        target_include_directories(${FWPROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs>)
        # Allows include of type <core/..> <data/..> ...
        target_include_directories(${FWPROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/core>)
        # Allows include of type <modules/../..>
        target_include_directories(${FWPROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>)

        if(SIGHT_ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
            if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
                add_precompiled_header(${FWPROJECT_NAME} pch.hpp)
                if(SIGHT_VERBOSE_PCH)
                    message(STATUS "Use custom precompiled header")
                endif()
            else()
                use_precompiled_header(${FWPROJECT_NAME} ${${FWPROJECT_NAME}_PCH_TARGET})
                if(SIGHT_VERBOSE_PCH)
                    message(STATUS "Use ${${FWPROJECT_NAME}_PCH_TARGET} precompiled header")
                endif()
            endif()
        endif()
    else()
        add_library(${FWPROJECT_NAME} INTERFACE ${${FWPROJECT_NAME}_RC_FILES} ${${FWPROJECT_NAME}_CMAKE_FILES})
    endif()
    
    if("${TARGET_TYPE}" STREQUAL "APP")
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "APP")
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "app")

        if(MSVC_IDE)
            # create the launch config for the current app
            set(LAUNCHER "${CMAKE_BINARY_DIR}/bin/sightrun.exe")
            set(PROFILE "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}/profile.xml")
            set(WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
            include(${FWCMAKE_RESOURCE_PATH}/install/win_install.cmake)
            if(CMAKE_CL_64)
                set(PLATFORM "x64")
            else()
                set(PLATFORM "Win32")
            endif()
            configure_file(
                "${CMAKE_SOURCE_DIR}/cmake/build/project.vcxproj.user.in"
                "${CMAKE_BINARY_DIR}/${FWPROJECT_NAME}/${FWPROJECT_NAME}.vcxproj.user"
                IMMEDIATE @ONLY)
        endif()
        if(UNIX)
            # Install shortcut
            string(TOLOWER ${FWPROJECT_NAME} APP_NAME)

            set(LAUNCHER "sightrun.bin")
            set(PROFILE_PATH "${FWPROJECT_NAME}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}")
            else()
                set(LAUNCHER_PATH "$me")
            endif()

            # Configure launcher script
            # Replace all ';' path separator to unix style path separator ':'
            if(SIGHT_EXTERNAL_LIBRARIES)
                string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${SIGHT_EXTERNAL_LIBRARIES}/lib")
            else()
                string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
            endif()

            configure_file(${FWCMAKE_RESOURCE_PATH}/build/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
            unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIRS)

            file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        elseif(WIN32)
            # Install shortcut
            string(TOLOWER ${FWPROJECT_NAME} APP_NAME)

            set(LAUNCHER "sightrun.exe")
            set(PROFILE_PATH "${FWPROJECT_NAME}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}\\${LAUNCHER}")
            else()
                set(LAUNCHER_PATH "%BINDIR%\\${LAUNCHER}")
            endif()

            file(TO_NATIVE_PATH "${PROFILE_PATH}" PROFILE_PATH)

            configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/template.bat.in ${CMAKE_BINARY_DIR}/bin/${APP_NAME}.bat @ONLY)
        endif()
    else()
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "MODULE")
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "module")                 

        set_target_properties(${FWPROJECT_NAME} PROPERTIES EXPORT_PROPERTIES "SIGHT_TARGET_TYPE;SIGHT_START")

        # Only prefix with the repository name for modules
        set_target_properties(${FWPROJECT_NAME} PROPERTIES OUTPUT_NAME ${SIGHT_REPOSITORY}_${FWPROJECT_NAME})

        install(
            TARGETS ${FWPROJECT_NAME}
            EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
            RUNTIME DESTINATION ${SIGHT_MODULE_LIB_PREFIX}
            ARCHIVE DESTINATION ${SIGHT_MODULE_LIB_PREFIX}
            LIBRARY DESTINATION ${SIGHT_MODULE_LIB_PREFIX}
        )
                
        # Add all targets to the build-tree export set
        export( EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
                FILE "${CMAKE_BINARY_DIR}/cmake/${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets.cmake"
                NAMESPACE ${SIGHT_REPOSITORY}::)

        # Install sight_Project_Targets.cmake
        install(EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
                FILE
                    ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets.cmake
                NAMESPACE
                    ${SIGHT_REPOSITORY}::
                DESTINATION
                    ${FWCONFIG_PACKAGE_LOCATION}
        )
        get_property(SIGHT_COMPONENTS GLOBAL PROPERTY SIGHT_COMPONENTS)
        set_property(GLOBAL PROPERTY SIGHT_COMPONENTS ${SIGHT_COMPONENTS};${FWPROJECT_NAME} )
    endif()

    set(${FWPROJECT_NAME}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")
    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        createResourcesTarget( ${FWPROJECT_NAME}_rc "${PRJ_SOURCE_DIR}/rc" "${${FWPROJECT_NAME}_RC_BUILD_DIR}" )
        add_dependencies( ${FWPROJECT_NAME} ${FWPROJECT_NAME}_rc )
    endif()

    createResourcesInstallTarget( "${${FWPROJECT_NAME}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}" )

    if(${FWPROJECT_NAME}_MODULE_DEPENDENCIES)
        message(WARNING "Module ${FWPROJECT_NAME} links with other modules (${${FWPROJECT_NAME}_MODULE_DEPENDENCIES}), "
                        "this feature will be removed in version 21.0 of Sight")
    endif()
endmacro()

function(getPchTarget TARGET TARGET_DIR TYPE PCH)

    if(NOT PCH STREQUAL "ON")
        # Use pch from an another target
        set(${TARGET}_PCH_TARGET ${PCH} PARENT_SCOPE)
    elseif(EXISTS "${TARGET_DIR}/pch.hpp")
        # Custom pch
        set(${TARGET}_PCH_TARGET ${TARGET} PARENT_SCOPE)
    else()
        # Default pch
        if( TYPE STREQUAL "MODULE" )
            set(${TARGET}_PCH_TARGET pchServices PARENT_SCOPE)
        else()
            set(${TARGET}_PCH_TARGET pchCore PARENT_SCOPE)
        endif()
    endif()

endfunction()


macro(sight_add_target)

    set(options)
    set(oneValueArgs TYPE PCH START PRIORITY CONSOLE OBJECT_LIBRARY WARNINGS_AS_ERRORS UNIQUE)
    set(multiValueArgs)
    cmake_parse_arguments(SIGHT_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(NAME ${ARGV0})
    
    message(STATUS "Configuring ${NAME}: ${CMAKE_CURRENT_SOURCE_DIR}")

    initProject(${NAME} ${SIGHT_TARGET_TYPE})

    if(NOT DEFINED SIGHT_TARGET_PCH)
        set(SIGHT_TARGET_PCH ON)
    elseif(NOT SIGHT_TARGET_PCH)
        set(${NAME}_DISABLE_PCH ON)
    endif()
    if(NOT DEFINED SIGHT_TARGET_OBJECT_LIBRARY)
        set(SIGHT_TARGET_OBJECT_LIBRARY OFF)
    endif()
    
    # Get the pch target, test the existence of type variable to exclude unbuilt projects
    if(SIGHT_ENABLE_PCH AND SIGHT_TARGET_TYPE AND SIGHT_TARGET_PCH)
        set(${NAME}_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

        getPchTarget(${NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${SIGHT_TARGET_TYPE} ${SIGHT_TARGET_PCH})
    endif()
    
    if("${SIGHT_TARGET_TYPE}" STREQUAL "EXECUTABLE")
        fwExec(${NAME})
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "LIBRARY")
        fwLib(${NAME} ${SIGHT_TARGET_OBJECT_LIBRARY})
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "MODULE")
        fwModule(${NAME} ${SIGHT_TARGET_TYPE})
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "TEST")
        fwCppunitTest(${NAME} "${OPTIONS}")
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "APP")
        fwModule(${NAME} ${SIGHT_TARGET_TYPE})
    endif()

    if("${SIGHT_TARGET_TYPE}" STREQUAL "APP")
        if(NOT DEFINED SIGHT_TARGET_UNIQUE)
            set(SIGHT_TARGET_UNIQUE FALSE)
        endif()
        set(SIGHT_TARGET_UNIQUE FALSE)
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_UNIQUE "${SIGHT_TARGET_UNIQUE}")
    endif()

    if("${SIGHT_TARGET_TYPE}" STREQUAL "MODULE" OR "${SIGHT_TARGET_TYPE}" STREQUAL "APP" AND SIGHT_TARGET_START)
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_START "${SIGHT_TARGET_START}")
    endif()

    if(SIGHT_TARGET_WARNINGS_AS_ERRORS)
        fwManageWarnings(${NAME})
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
        include("Dependencies.cmake")
    endif()

    # Generate batch script to ease the set of PATH in order to launch a Sight application on Windows.
    if(WIN32)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/setpath.bat.in ${CMAKE_BINARY_DIR}/bin/setpath.bat @ONLY)
    endif()

    if("${SIGHT_TARGET_TYPE}" STREQUAL "EXECUTABLE" OR "${SIGHT_TARGET_TYPE}" STREQUAL "APP")
        generic_install()
    endif()
endmacro()

macro(sight_generate_profile TARGET)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rc/profile.xml" )
        profile_setup(${TARGET})
    endif()
endmacro()

# Treat warnings as errors if requested
#   to activate "warning as errors", simply write in the Properties.cmake of your project:
#   set(WARNINGS_AS_ERRORS ON)
macro(fwManageWarnings PROJECT)
    if(${${PROJECT}_WARNINGS_AS_ERRORS})
        if(MSVC)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
                # wd4996: deprecated declaration will be displayed as warning and not errors
                target_compile_options(${PROJECT} PRIVATE /WX /wd4996)
            else()
                message(WARNING "Your version of MSVC is too old to use WARNINGS_AS_ERRORS.")
            endif()
        elseif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            # deprecated declaration will be displayed as warning and not errors
            target_compile_options(${PROJECT} PRIVATE "-Werror" "-Wno-error=deprecated-declarations")
        endif ()
    endif()
endmacro()

# Scans specified directory  ('curdir') and returns a list of subdirectories ('result').
macro(fwDirList result curdir)
    file(GLOB_RECURSE children LIST_DIRECTORIES true ${curdir}/*)
    unset(dirlist)
    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            list(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

function(findAllDependencies FWPROJECT_NAMES RESULT_VAR)
    set(DEPENDENCY_LIST)
    set(RESULT "")
    list(APPEND DEPENDENCY_LIST ${FWPROJECT_NAMES})
    while(DEPENDENCY_LIST)

        list(GET DEPENDENCY_LIST 0 DEPENDENCY)
        list(REMOVE_AT DEPENDENCY_LIST 0 )

        if(NOT PROCESSED_${DEPENDENCY})
            get_target_property(DEPENDS ${DEPENDENCY} LINK_LIBRARIES)
            set(DEPENDS_COPY ${DEPENDS})
            foreach(dep ${DEPENDS})
                if(NOT ${dep} IN_LIST FWPROJECT_NAMES)
                    list(REMOVE_ITEM DEPENDS_COPY ${dep})
                endif()
            endforeach()
            
            list(APPEND DEPENDENCY_LIST ${DEPENDS_COPY})
            set(PROCESSED_${DEPENDENCY} 1)
        endif()

        list(APPEND RESULT ${DEPENDENCY})
    endwhile()

    list(REMOVE_DUPLICATES RESULT)
    set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()