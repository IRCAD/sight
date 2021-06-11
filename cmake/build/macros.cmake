set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

if(WIN32)
    # Do not automatically copy VCPKG dependencies into the output directory
    set(VCPKG_APPLOCAL_DEPS OFF)
endif()

# Define global definitions for some external libraries

# Boost

add_definitions(
    -DBOOST_ALL_DYN_LINK
    -DBOOST_THREAD_DONT_PROVIDE_DEPRECATED_FEATURES_SINCE_V3_0_0
    -DBOOST_THREAD_PROVIDES_FUTURE
    -DBOOST_THREAD_VERSION=2
    -DBOOST_SPIRIT_USE_PHOENIX_V3
)
# Qt

#Fix error with BOOST_JOIN and qt moc
set(CMAKE_AUTOMOC_MOC_OPTIONS "-DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION")

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
    if(FW_BUILD_EXTERNAL)
        if(WIN32)
            set(FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${Sight_BINARY_DIR}")
        else()
            set(FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${Sight_LIBRARY_DIR}/..")
        endif()
    endif()

    if(WIN32)
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(SIGHT_VCPKG_RUNTIME_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin")
        else()
            set(SIGHT_VCPKG_RUNTIME_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
        endif()
    endif()
endmacro()
    
include(${FWCMAKE_INSTALL_FILES_DIR}/helper.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/plugin_config.cmake)
include(${FWCMAKE_BUILD_FILES_DIR}/profile_config.cmake)
include(${FWCMAKE_INSTALL_FILES_DIR}/generic_install.cmake)
include(${FWCMAKE_INSTALL_FILES_DIR}/get_git_rev.cmake)

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
    # apps -> project
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
    project( ${PRJ_NAME} )

    set(FWPROJECT_NAME ${PRJ_NAME})
    set(PRJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set(${FWPROJECT_NAME}_HEADERS)
    set(${FWPROJECT_NAME}_SOURCES)

    file(GLOB_RECURSE HEADERS "${PRJ_SOURCE_DIR}/*.hpp" "${PRJ_SOURCE_DIR}/*.h" "${PRJ_SOURCE_DIR}/*.hxx")
    file(GLOB_RECURSE SOURCES "${PRJ_SOURCE_DIR}/*.cpp" "${PRJ_SOURCE_DIR}/*.c" "${PRJ_SOURCE_DIR}/*.cxx")
    
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


macro(configureProject FWPROJECT_NAME )
    string(TOUPPER ${FWPROJECT_NAME} PROJECT_NAME_UPCASE)

    if(TARGET_OBJECT_LIB)
        set(BUILD_TARGET_NAME ${TARGET_OBJECT_LIB})
    else()
        set(BUILD_TARGET_NAME ${FWPROJECT_NAME})
    endif()

    set_target_properties(${FWPROJECT_NAME} PROPERTIES VERSION ${SIGHT_VERSION} SOVERSION ${SIGHT_API_VERSION})

    target_compile_definitions(${BUILD_TARGET_NAME} PRIVATE "${PROJECT_NAME_UPCASE}_EXPORTS")

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
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

    # Adds project into folder exec
    set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "exec")
endmacro()


macro(fwCppunitTest FWPROJECT_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(fwCppunitTest "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${FWPROJECT_NAME}_PCH_TARGET} STREQUAL ${FWPROJECT_NAME})
            add_precompiled_header_cpp(${FWPROJECT_NAME})
        endif()
        set(${FWPROJECT_NAME}_PCH_LIB $<TARGET_OBJECTS:${${FWPROJECT_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    configure_file(
        "${FWCMAKE_RESOURCE_PATH}/build/cppunit_main.cpp"
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
        ${${FWPROJECT_NAME}_CMAKE_FILES}
        ${${FWPROJECT_NAME}_PCH_LIB}
    )

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
        if(UNIX)
            # CMAKE_POSITION_INDEPENDENT_CODE sets "-fPIE" but we also needs the "-fPIC" used in the PCH
            target_compile_options (${FWPROJECT_NAME} PRIVATE "-fPIC")
        endif()
    endif()

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
    
    if(${OBJECT_LIBRARY})
        set(TARGET_OBJECT_LIB ${FWPROJECT_NAME}_obj)
        set(TARGET_NAME ${TARGET_OBJECT_LIB})
    else()
        set(TARGET_NAME ${FWPROJECT_NAME})
    endif()

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${TARGET_NAME}_PCH_TARGET} STREQUAL ${TARGET_NAME})
            add_precompiled_header_cpp(${TARGET_NAME})
        endif()
        set(${TARGET_NAME}_PCH_LIB $<TARGET_OBJECTS:${${TARGET_NAME}_PCH_TARGET}_PCH_OBJ>)
    endif()

    if(${OBJECT_LIBRARY})

        add_library(${TARGET_OBJECT_LIB} OBJECT
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            $<BUILD_INTERFACE:${${FWPROJECT_NAME}_PCH_LIB}>)

        add_library(${FWPROJECT_NAME} SHARED $<TARGET_OBJECTS:${TARGET_OBJECT_LIB}> $<BUILD_INTERFACE:${${TARGET_NAME}_PCH_LIB}>)

        target_include_directories(${TARGET_OBJECT_LIB} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/core/>
        )
        target_include_directories(${FWPROJECT_NAME} PUBLIC
            $<INSTALL_INTERFACE:include>
        )
        target_link_libraries(${FWPROJECT_NAME} PUBLIC ${TARGET_OBJECT_LIB})
    else()
        add_library(${FWPROJECT_NAME} SHARED
            ${${FWPROJECT_NAME}_HEADERS}
            ${${FWPROJECT_NAME}_SOURCES}
            ${${FWPROJECT_NAME}_RC_FILES}
            ${${FWPROJECT_NAME}_CMAKE_FILES}
            $<BUILD_INTERFACE:${${FWPROJECT_NAME}_PCH_LIB}>)
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
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_MODULE_RC_DIR "\${_IMPORT_PREFIX}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")
    endif()

    # create the config.hpp for the current library
    get_header_file_install_destination()
    configure_header_file(${FWPROJECT_NAME} "config.hpp" "${HEADER_FILE_DESTINATION_REL}")

    # export and install target
    if(NOT ${FWPROJECT_NAME} MATCHES "^pch.*")
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${HEADER_FILE_DESTINATION_REL}
                FILES_MATCHING PATTERN "*.h"
                                PATTERN "*.hpp"
                                PATTERN "*.hxx"
                                PATTERN "test/*" EXCLUDE)
        set(TARGETS_TO_EXPORT ${FWPROJECT_NAME})

        if(${OBJECT_LIBRARY})
            set(TARGETS_TO_EXPORT ${FWPROJECT_NAME} ${TARGET_OBJECT_LIB})
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
        set_target_properties(${FWPROJECT_NAME} PROPERTIES EXPORT_PROPERTIES "SIGHT_MODULE_RC_DIR")
    endif()

    if(SIGHT_ENABLE_PCH AND NOT ${FWPROJECT_NAME}_DISABLE_PCH)
        if(${${TARGET_NAME}_PCH_TARGET} STREQUAL ${TARGET_NAME})
            add_precompiled_header(${TARGET_NAME} pch.hpp)
            if(SIGHT_VERBOSE_PCH)
                message(STATUS "Use custom precompiled header")
            endif()
        else()
            use_precompiled_header(${TARGET_NAME} ${${TARGET_NAME}_PCH_TARGET})
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
            install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/${APP_NAME}.bat DESTINATION bin)
        endif()
    else()
        set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_TARGET_TYPE "MODULE")
        set_target_properties(${FWPROJECT_NAME} PROPERTIES FOLDER "module")                 

        # Only prefix with the repository name for modules
        set_target_properties(${FWPROJECT_NAME} PROPERTIES OUTPUT_NAME ${SIGHT_REPOSITORY}_${FWPROJECT_NAME})

        install(
            TARGETS ${FWPROJECT_NAME}
            EXPORT ${SIGHT_REPOSITORY}_${FWPROJECT_NAME}_Targets
            RUNTIME DESTINATION ${SIGHT_MODULE_LIB_PREFIX}
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
    set_target_properties(${FWPROJECT_NAME} PROPERTIES SIGHT_MODULE_RC_DIR "\${_IMPORT_PREFIX}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")

    set_target_properties(${FWPROJECT_NAME} PROPERTIES EXPORT_PROPERTIES "SIGHT_TARGET_TYPE;SIGHT_START;SIGHT_MODULE_RC_DIR;SIGHT_MODULE_DEPENDENCIES")

    if(${FWPROJECT_NAME}_MODULE_DEPENDENCIES)
        message(WARNING "Module ${FWPROJECT_NAME} links with other modules (${${FWPROJECT_NAME}_MODULE_DEPENDENCIES}), "
                        "this feature will be removed in version 21.0 of Sight")
    endif()
endmacro()

function(getPchTarget TARGET TARGET_DIR TYPE PCH OBJECT_LIBRARY)

    if(${OBJECT_LIBRARY})
        set(TARGET_NAME ${TARGET}_obj)
    else()
        set(TARGET_NAME ${TARGET})
    endif()

    if(NOT PCH STREQUAL "ON")
        # Use pch from an another target
        set(${TARGET_NAME}_PCH_TARGET ${PCH} PARENT_SCOPE)
    elseif(EXISTS "${TARGET_DIR}/pch.hpp")
        # Custom pch
        set(${TARGET_NAME}_PCH_TARGET ${TARGET_NAME} PARENT_SCOPE)
    else()
        # Default pch
        if( TYPE STREQUAL "MODULE" )
            set(${TARGET_NAME}_PCH_TARGET pchServices PARENT_SCOPE)
        else()
            set(${TARGET_NAME}_PCH_TARGET pchCore PARENT_SCOPE)
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
        getPchTarget(${NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${SIGHT_TARGET_TYPE} ${SIGHT_TARGET_PCH} ${SIGHT_TARGET_OBJECT_LIBRARY})
    endif()
    
    if("${SIGHT_TARGET_TYPE}" STREQUAL "EXECUTABLE")
        fwExec(${NAME} CONSOLE ${SIGHT_TARGET_CONSOLE})
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

# Find link and manual dependencies for a target
# All targets that do not appear in FILTERED_COMPONENTS is excluded
function(findTargetDependencies TARGET TARGETS_FILTER RESULT_VAR)
    set(DEPENDENCY_LIST)
    set(RESULT "")
    list(APPEND DEPENDENCY_LIST ${TARGET})
    while(DEPENDENCY_LIST)

        list(GET DEPENDENCY_LIST 0 DEPENDENCY)
        list(REMOVE_AT DEPENDENCY_LIST 0 )

        if(NOT PROCESSED_${DEPENDENCY})
            get_target_property(DEPENDS ${DEPENDENCY} LINK_LIBRARIES)
            get_target_property(INTERFACE_DEPENDS ${DEPENDENCY} INTERFACE_LINK_LIBRARIES)
            list(APPEND DEPENDS ${INTERFACE_DEPENDS})
            set(DEPENDS_COPY ${DEPENDS})
            foreach(dep ${DEPENDS})
                if(NOT ${dep} IN_LIST TARGETS_FILTER)
                    list(REMOVE_ITEM DEPENDS_COPY ${dep})
                endif()
            endforeach()
            
            list(APPEND DEPENDENCY_LIST ${DEPENDS_COPY})

            get_target_property(DEPENDS ${DEPENDENCY} MANUALLY_ADDED_DEPENDENCIES) 
            get_target_property(MODULE_DEPENDS ${DEPENDENCY} SIGHT_MODULE_DEPENDENCIES)
            list(APPEND DEPENDS ${MODULE_DEPENDS})
            list(FILTER DEPENDS EXCLUDE REGEX "-NOTFOUND")
            list(REMOVE_DUPLICATES DEPENDS)
            set(DEPENDS_COPY ${DEPENDS})
            foreach(dep ${DEPENDS})
                if(NOT ${dep} IN_LIST TARGETS_FILTER AND NOT "${dep}" STREQUAL "sightrun")
                    list(REMOVE_ITEM DEPENDS_COPY ${dep})
                endif()
            endforeach()
            if(DEPENDS)
                list(APPEND DEPENDENCY_LIST ${DEPENDS_COPY})
            endif()

            set(PROCESSED_${DEPENDENCY} 1)
        endif()

        list(APPEND RESULT ${DEPENDENCY})
    endwhile()

    list(REMOVE_DUPLICATES RESULT)
    list(REMOVE_ITEM RESULT ${TARGET})
    set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

# This function must be called at the end of the main CMakeLists.txt 
# in order to create an APP_install and APP_package for each Sight target of type APP or EXEC
function(sight_create_package_targets SIGHT_COMPONENTS SIGHT_IMPORTED_COMPONENTS)

    # Add an install target for every component
    foreach(COMPONENT ${SIGHT_COMPONENTS})
        if(NOT ${COMPONENT} MATCHES "_obj")
            get_target_property(COMPONENT_BINARY_DIR ${COMPONENT} BINARY_DIR)
            add_custom_target(${COMPONENT}_install
                                ${CMAKE_COMMAND} -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -P ${COMPONENT_BINARY_DIR}/cmake_install.cmake)
            add_dependencies(${COMPONENT}_install ${COMPONENT})
        endif()
        get_target_property(DEPENDENCIES ${COMPONENT} MANUALLY_ADDED_DEPENDENCIES)
        list(FILTER DEPENDENCIES EXCLUDE REGEX "_rc")
        list(TRANSFORM DEPENDENCIES PREPEND ${SIGHT_REPOSITORY}::)
        set_target_properties(${COMPONENT} PROPERTIES SIGHT_MODULE_DEPENDENCIES "${DEPENDENCIES}")
    endforeach()

    if(WIN32)
        # Might be needed later to get ${OGRE_PLUGIN_DIR}
        find_package(OGRE REQUIRED)
    endif()
    get_property(SIGHT_APPS GLOBAL PROPERTY SIGHT_APPS)
    foreach(APP ${SIGHT_APPS})
        get_target_property(APP_BINARY_DIR ${APP} BINARY_DIR)
        set(IMPORTED_RC_DIRS "")
        set(IMPORTED_LIBS "")
        
        findTargetDependencies(${APP} "${SIGHT_IMPORTED_COMPONENTS}" IMPORTED_DEPENDS)
        foreach(DEP ${IMPORTED_DEPENDS})
            get_target_property(CONFIG ${DEP} IMPORTED_CONFIGURATIONS)
            get_target_property(LIB ${DEP} IMPORTED_LOCATION_${CONFIG})
            if(LIB)
                list(APPEND IMPORTED_LIBS ${LIB})
            endif()

            get_target_property(RC_DIR ${DEP} SIGHT_MODULE_RC_DIR)
            if(RC_DIR)
                list(APPEND IMPORTED_RC_DIRS ${RC_DIR})
            endif()
        endforeach()

        list(REMOVE_DUPLICATES IMPORTED_RC_DIRS)
        list(REMOVE_DUPLICATES IMPORTED_LIBS)

        # Add an install target for every app
        add_custom_target(${APP}_install
                          ${CMAKE_COMMAND} -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -P ${APP_BINARY_DIR}/cmake_install.cmake
        )
        add_dependencies(${APP}_install ${APP})
        
        if(IMPORTED_RC_DIRS OR IMPORTED_LIBS)
            configure_file(${FWCMAKE_RESOURCE_PATH}/install/install_imported.cmake.in ${APP_BINARY_DIR}/install_imported.cmake @ONLY)

            # Add an install target that installs imported components
            add_custom_target(${APP}_install_imported
                            ${CMAKE_COMMAND} -P ${APP_BINARY_DIR}/install_imported.cmake
                            COMMENT "Install imported targets..."
            )
            add_dependencies(${APP}_install ${APP}_install_imported)
        endif()

        findTargetDependencies(${APP} "${SIGHT_COMPONENTS}" DEPENDS)
        foreach(DEP ${DEPENDS})
            if(NOT ${DEP} MATCHES "_obj")
                add_dependencies(${APP}_install ${DEP}_install) 
            endif()
        endforeach()
        
        # Add a fixup target for every app
        if(WIN32)
            list(APPEND DEPENDS ${IMPORTED_DEPENDS})
            add_custom_target(${APP}_install_plugins
                ${CMAKE_COMMAND} -DDEPENDS="${DEPENDS}"
                                 -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
                                 -DQT_PLUGINS_SOURCE_DIR="${Qt5_DIR}/../../..$<$<CONFIG:Debug>:/debug>/plugins" 
                                 -DQT_DESTINATION="${CMAKE_INSTALL_BINDIR}/.."
                                 -DOGRE_PLUGIN_DIR="${OGRE_PLUGIN_DIR}"
                                 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
                                 -P "${FWCMAKE_RESOURCE_PATH}/install/windows/install_plugins.cmake"
                COMMENT "Install plugins..."
            )
            add_dependencies(${APP}_install_plugins ${APP}_install)

            # Used in windows_fixup.cmake.in
            if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                set(SIGHT_VCPKG_ROOT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug")
            else()
                set(SIGHT_VCPKG_ROOT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
                set(EXCLUDE_PATTERN ".*/debug/.*")
            endif()
            set(LAUNCHER_PATH "${CMAKE_INSTALL_BINDIR}/sightrun.exe")

            configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/windows_fixup.cmake.in ${CMAKE_BINARY_DIR}/windows_fixup.cmake @ONLY)
            add_custom_target(${APP}_fixup
                            ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/windows_fixup.cmake
                            COMMENT "Fixup before packaging...")
            add_dependencies(${APP}_fixup ${APP}_install ${APP}_install_plugins)
        endif()

        # Add a package target for every app
        add_custom_target(${APP}_package
                          ${CMAKE_CPACK_COMMAND} --config ${APP_BINARY_DIR}/CPackConfig.cmake)
        add_dependencies(${APP}_package ${APP}_install)
        if(WIN32)
            add_dependencies(${APP}_package ${APP}_fixup)
        endif()

    endforeach()

    get_platform_package_suffix()
    if(UNIX)
        set(CPACK_GENERATOR TZST)
        set(CPACK_SOURCE_GENERATOR TZST)
    else()
        set(CPACK_GENERATOR ZIP)
        set(CPACK_SOURCE_GENERATOR ZIP)
    endif()

    # Whole library packaging    
    set(CPACK_OUTPUT_FILE_PREFIX packages)
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") # look inside install dir for packaging

    set(CPACK_PACKAGE_FILE_NAME "sight-${GIT_TAG}-${PLATFORM_SUFFIX}")
    set(CPACK_PACKAGE_VENDOR "IRCAD")
    set(CPACK_PACKAGE_NAME "Sight")
    set(CPACK_PACKAGE_VERSION "${SIGHT_VERSION}")
    set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake")
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake")
    include(CPack)

endfunction()

# Order sight component list, according to build dependency tree
function(order_sight_components SIGHT_UNORDERED_COMPONENTS SIGHT_ORDERED_COMPONENTS)
    set(unordered_components ${SIGHT_UNORDERED_COMPONENTS})
    set(ordered_components)

    while(unordered_components)
        foreach(component ${unordered_components})
            set(resolved TRUE)

            # Find all component dependencies
            findTargetDependencies(${component} "${SIGHT_UNORDERED_COMPONENTS}" dependencies)

            # Try to guess if we have an "_obj" target, in which case we must also add the "_obj" dependencies
            set(component_obj "${component}_obj")
            if(TARGET ${component_obj})
                findTargetDependencies(${component_obj} "${SIGHT_UNORDERED_COMPONENTS}" component_dependencies)
                list(APPEND dependencies ${component_dependencies})
            endif()

            # If we have dependencies, we check if all of them are not already in the ordered list
            if(dependencies)
                foreach(dependency ${dependencies})
                    if(NOT ${dependency} IN_LIST ordered_components)
                        set(resolved FALSE)
                        break()
                    endif()
                endforeach()
            endif()

            # If all dependencies are already resolved, the component is also resolved
            # Append it to the final list
            if(resolved)
                list(APPEND ordered_components ${component})
                list(REMOVE_ITEM unordered_components ${component})
            endif()
        endforeach()
    endwhile()

    set(SIGHT_ORDERED_COMPONENTS ${ordered_components} PARENT_SCOPE)
endfunction()

# Copy Ogre plugins in the build folder.
# It is necessary for applications to run from the build directory.
# For packaged applications, we use install_plugins.cmake
macro(copy_ogre_plugins)
    # Fixup VCPKG paths
    if(WIN32)
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            if(EXISTS "${OGRE_PLUGIN_DIR}/../debug/bin")
                get_filename_component(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR}/../debug/bin" ABSOLUTE)
            endif()
        endif()

        file(GLOB OGRE_PLUGINS
            "${OGRE_PLUGIN_DIR}/*RenderSystem*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
            "${OGRE_PLUGIN_DIR}/*Plugin_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
            "${OGRE_PLUGIN_DIR}/*Codec_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        )

        set(FW_OGRE_PLUGINS_DIR "${CMAKE_BINARY_DIR}/ogreplugins/")

        # This copies the plugins into the build directory
        file(INSTALL ${OGRE_PLUGINS} DESTINATION "${FW_OGRE_PLUGINS_DIR}")
        message(STATUS "Copying Ogre Plugins ['${OGRE_PLUGINS}'] to: '${FW_OGRE_PLUGINS_DIR}'")
    endif()
endmacro()