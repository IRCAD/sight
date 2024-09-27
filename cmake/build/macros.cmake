set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

if(WIN32)
    # Do not automatically copy VCPKG dependencies into the output directory
    set(VCPKG_APPLOCAL_DEPS OFF)
endif()

# Define global definitions for some external libraries

# Boost

add_definitions(
    -DBOOST_ALL_DYN_LINK -DBOOST_THREAD_DONT_PROVIDE_DEPRECATED_FEATURES_SINCE_V3_0_0 -DBOOST_THREAD_PROVIDES_FUTURE
    -DBOOST_THREAD_VERSION=2 -DBOOST_SPIRIT_USE_PHOENIX_V3
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
macro(set_external_libraries_dir)
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
if(UNIX)
    include(${FWCMAKE_BUILD_FILES_DIR}/linux/manpage.cmake)
endif()

# Get the privilege excalation string
macro(get_admin_request_string ADMIN_REQUEST)
    string(
        CONCAT ${ADMIN_REQUEST}
               "rem This executable was marked as requiring admin access.\n"
               "rem Check if we have elevated privileges\n"
               "whoami /all | findstr S-1-16-12288 > nul\n"
               "rem if we do not have those, restart the script as Admin\n"
               "if %errorlevel%==1 if not \"%~1\"==\"/noadmin\" "
               "(\n"
               "    if \"%~1\" == \"\" (\n"
               "        powershell Start-Process -verb runas '%~f0' & exit /b\n"
               "    ) else (\n"
               "        powershell Start-Process -verb runas -ArgumentList '%*' '%~f0' & exit /b\n"
               "    )\n"
               ")\n"
    )
endmacro()

# Create the target sources group
macro(group_maker SIGHT_TARGET)
    file(GLOB_RECURSE PRJ_SOURCES "${${SIGHT_TARGET}_DIR}/*")
    foreach(SRC ${PRJ_SOURCES})
        string(REGEX REPLACE ${${SIGHT_TARGET}_DIR} "" REL_DIR "${SRC}")
        string(REGEX REPLACE "[\\\\/][^\\\\/]*$" "" REL_DIR "${REL_DIR}")
        string(REGEX REPLACE "^[\\\\/]" "" REL_DIR "${REL_DIR}")
        string(REPLACE "/" "\\" REL_DIR "${REL_DIR}")
        source_group("${REL_DIR}" FILES ${SRC})
    endforeach()

    file(GLOB_RECURSE PRJ_BUILD_SOURCES "${${SIGHT_TARGET}_BUILD_DIR}/*.cpp" "${${SIGHT_TARGET}_BUILD_DIR}/*.hpp")
    foreach(SRC ${PRJ_BUILD_SOURCES})
        source_group("genFiles" FILES ${SRC})
    endforeach()
endmacro()

# Set the header installation directory
function(get_header_file_install_destination)
    # Paths for config files are:
    # activities -> activity/theme/project/
    # apps -> project
    # examples -> project
    # libs -> theme/project/ except for theme=main project
    # modules -> modules/theme/project/  except for theme=main modules/project
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
        string(REPLACE "__" "." THEME ${THEME})
        set(PROJECT_PATH "${THEME}/${PROJECT}")
    endif()

    if("${ROOT}" STREQUAL "libs")
        set(HEADER_FILE_DESTINATION_REL "${PROJECT_PATH}")
    else()
        if("${ROOT}" STREQUAL "modules" OR "${ROOT}" STREQUAL "activities")
            set(HEADER_FILE_DESTINATION_REL "${ROOT}/${PROJECT_PATH}")
        else()
            set(HEADER_FILE_DESTINATION_REL "${PROJECT}")
        endif()
    endif()
    set(HEADER_FILE_DESTINATION_REL "${HEADER_FILE_DESTINATION_REL}" PARENT_SCOPE)
    set(HEADER_FILE_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/${HEADER_FILE_DESTINATION_REL}"
        PARENT_SCOPE
    )
endfunction()

# Configure header template
macro(configure_header_file SIGHT_TARGET FILENAME HEADER_FILE_DESTINATION HEADER_FILE_DESTINATION_REL)

    configure_file("${FWCMAKE_BUILD_FILES_DIR}/${FILENAME}.in" ${HEADER_FILE_DESTINATION}/${FILENAME} IMMEDIATE @ONLY)

    install(FILES ${HEADER_FILE_DESTINATION}/${FILENAME}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${HEADER_FILE_DESTINATION_REL}
    )
endmacro()

# Initialize the project and set basic variables
macro(init_project PRJ_NAME PRJ_TYPE)
    set(SIGHT_TARGET ${PRJ_NAME})
    set(PRJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set(${SIGHT_TARGET}_HEADERS)
    set(${SIGHT_TARGET}_SOURCES)

    file(GLOB_RECURSE HEADERS "${PRJ_SOURCE_DIR}/*.hpp" "${PRJ_SOURCE_DIR}/*.h" "${PRJ_SOURCE_DIR}/*.hxx"
         "${PRJ_SOURCE_DIR}/*.cuh"
    )
    file(GLOB_RECURSE SOURCES "${PRJ_SOURCE_DIR}/*.cpp" "${PRJ_SOURCE_DIR}/*.c" "${PRJ_SOURCE_DIR}/*.cxx"
         "${PRJ_SOURCE_DIR}/*.cu"
    )

    if(NOT "${PRJ_TYPE}" STREQUAL "TEST" AND NOT "${PRJ_TYPE}" STREQUAL "GUI_TEST")
        list(FILTER SOURCES EXCLUDE REGEX "/test/api")
        list(FILTER SOURCES EXCLUDE REGEX "/test/detail")
        list(FILTER SOURCES EXCLUDE REGEX "/test/tu")
        list(FILTER SOURCES EXCLUDE REGEX "/test/ui")
        list(FILTER SOURCES EXCLUDE REGEX "/test/uit")
        list(FILTER SOURCES EXCLUDE REGEX "/test/ut")
        list(FILTER HEADERS EXCLUDE REGEX "/test/api")
        list(FILTER HEADERS EXCLUDE REGEX "/test/detail")
        list(FILTER HEADERS EXCLUDE REGEX "/test/tu")
        list(FILTER HEADERS EXCLUDE REGEX "/test/ui")
        list(FILTER HEADERS EXCLUDE REGEX "/test/uit")
        list(FILTER HEADERS EXCLUDE REGEX "/test/ut")
    endif()

    list(APPEND ${SIGHT_TARGET}_HEADERS ${HEADERS})
    list(APPEND ${SIGHT_TARGET}_SOURCES ${SOURCES})

    set(${SIGHT_TARGET}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(${SIGHT_TARGET}_DIR ${${SIGHT_TARGET}_DIR} PARENT_SCOPE)
    set(${SIGHT_TARGET}_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(${SIGHT_TARGET}_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

    set(${SIGHT_TARGET}_HEADERS ${${SIGHT_TARGET}_HEADERS} PARENT_SCOPE)
    set(${SIGHT_TARGET}_SOURCES ${${SIGHT_TARGET}_SOURCES} PARENT_SCOPE)

    file(GLOB_RECURSE ${SIGHT_TARGET}_RC_FILES "${PRJ_SOURCE_DIR}/rc/*" "${PRJ_SOURCE_DIR}/ut/rc/*"
         "${PRJ_SOURCE_DIR}/tu/rc/*"
    )
    set(${SIGHT_TARGET}_RC_FILES ${${SIGHT_TARGET}_RC_FILES} PARENT_SCOPE)
    set_source_files_properties(${${SIGHT_TARGET}_RC_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    file(GLOB ${SIGHT_TARGET}_CMAKE_FILES "${PRJ_SOURCE_DIR}/*.txt" "${PRJ_SOURCE_DIR}/*.cmake")
    set(${SIGHT_TARGET}_CMAKE_FILES ${${SIGHT_TARGET}_CMAKE_FILES} PARENT_SCOPE)
    set_source_files_properties(${${SIGHT_TARGET}_CMAKE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

    group_maker(${SIGHT_TARGET})
endmacro()

# Configure the project
macro(configure_project SIGHT_TARGET)
    string(TOUPPER ${PROJECT_NAME}_${SIGHT_TARGET} TARGET_NAME_UPCASE)

    if(TARGET_OBJECT_LIB)
        set(BUILD_TARGET_NAME ${TARGET_OBJECT_LIB})
    else()
        set(BUILD_TARGET_NAME ${SIGHT_TARGET})
    endif()

    target_compile_definitions(${BUILD_TARGET_NAME} PRIVATE "${TARGET_NAME_UPCASE}_EXPORTS")

    # Get CMake target type (not Sight one)
    get_target_property(TARGET_TYPE ${SIGHT_TARGET} TYPE)

    if(NOT ${TARGET_TYPE} MATCHES "EXECUTABLE")
        set(${SIGHT_TARGET}_LIBRARY "$<TARGET_FILE:${SIGHT_TARGET}>")
        set(${SIGHT_TARGET}_LIBRARY ${${SIGHT_TARGET}_LIBRARY} PARENT_SCOPE)
    endif()

endmacro()

# Create a target for the resources
macro(create_resources_target TARGET TARGET_RC RES_DIR TARGET_RC_DIR)
    file(GLOB_RECURSE RESOURCES_FILES "${RES_DIR}/*")
    set(CREATED_RESOURCES_LIST)
    foreach(RESOURCE_FILE ${RESOURCES_FILES})
        file(RELATIVE_PATH REL_PATH "${RES_DIR}" "${RESOURCE_FILE}")
        if("${TARGET_RC_DIR}/${REL_PATH}" MATCHES "^.*\\.(txt|xml|cfg)$")
            set(COPY_COMMAND
                ${CMAKE_COMMAND} -DIN_FILE="${RES_DIR}/${REL_PATH}" -DOUT_FILE="${TARGET_RC_DIR}/${REL_PATH}"
                -DFWPROJECT_NAME="${SIGHT_TARGET_RC}" -D${TARGET}_VERSION="${${TARGET}_VERSION}"
                -DPROJECT_VERSION="${PROJECT_VERSION}" -P ${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake
            )
            set(COPY_DEPENDS "${FWCMAKE_RESOURCE_PATH}/build/configure_file.cmake")
        else()

            set(COPY_COMMAND ${CMAKE_COMMAND} -E copy "${RES_DIR}/${REL_PATH}" "${TARGET_RC_DIR}/${REL_PATH}")
            set(COPY_DEPENDS)
        endif()

        add_custom_command(
            OUTPUT "${TARGET_RC_DIR}/${REL_PATH}"
            COMMAND ${COPY_COMMAND}
            DEPENDS "${RES_DIR}/${REL_PATH}" ${COPY_DEPENDS}
            COMMENT "Copy resource ${RESOURCE_FILE}"
        )
        list(APPEND CREATED_RESOURCES_LIST "${TARGET_RC_DIR}/${REL_PATH}")

    endforeach()

    add_custom_target("${TARGET_RC}" DEPENDS ${CREATED_RESOURCES_LIST} COMMENT "Copy resources")

    # Adds project into folder rc
    set_target_properties("${TARGET_RC}" PROPERTIES FOLDER "rc")

    unset(CREATED_RESOURCES_LIST)
endmacro()

# Create a resource installation target
macro(create_resources_install_target CONFIGURED_FILES_DIR DESTINATION)
    install(DIRECTORY "${CONFIGURED_FILES_DIR}/" DESTINATION "${DESTINATION}/")
endmacro()

# Create an executable target
macro(fw_exec SIGHT_TARGET)
    set(options CONSOLE REQUIRE_ADMIN)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(FWEXEC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(WIN32 AND NOT ${FWEXEC_CONSOLE})
        set(GUI_TYPE WIN32)
    else()
        add_definitions(-D_CONSOLE)
    endif()

    add_executable(
        ${SIGHT_TARGET} ${GUI_TYPE} ${${SIGHT_TARGET}_HEADERS} ${${SIGHT_TARGET}_SOURCES} ${${SIGHT_TARGET}_RC_FILES}
                        ${${SIGHT_TARGET}_CMAKE_FILES}
    )

    set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_TARGET_TYPE "EXECUTABLE")

    # On linux add ".bin" suffix, to distinguish executable (.bin) from launcher script(.sh).
    if(UNIX)
        set_target_properties(${SIGHT_TARGET} PROPERTIES SUFFIX ".bin")
    endif()

    configure_project(${SIGHT_TARGET})

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${SIGHT_TARGET}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
        create_resources_target(
            ${SIGHT_TARGET} ${SIGHT_TARGET}_rc "${PRJ_SOURCE_DIR}/rc" "${${SIGHT_TARGET}_RC_BUILD_DIR}"
        )
        add_dependencies(${SIGHT_TARGET} ${SIGHT_TARGET}_rc)

        create_resources_install_target("${${SIGHT_TARGET}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
    endif()

    # Configure launcher script
    if(UNIX)

        string(TOLOWER ${SIGHT_TARGET} ${SIGHT_TARGET}_SCRIPT)
        set(PROJECT_EXECUTABLE "${SIGHT_TARGET}.bin")

        # Use the right path separator on unix
        if(SIGHT_EXTERNAL_LIBRARIES)
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${SIGHT_EXTERNAL_LIBRARIES}/lib")
        else()
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
        endif()

        # Build the shell script from template_exe.sh.in
        configure_file(
            ${FWCMAKE_RESOURCE_PATH}/build/linux/template_exe.sh.in
            ${CMAKE_CURRENT_BINARY_DIR}/${${SIGHT_TARGET}_SCRIPT} @ONLY
        )

        # Cleanup
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIR)

        file(
            COPY ${CMAKE_CURRENT_BINARY_DIR}/${${SIGHT_TARGET}_SCRIPT}
            DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        )

        add_man_page(${SIGHT_TARGET})

    elseif(WIN32)
        string(TOLOWER ${SIGHT_TARGET}.bat ${SIGHT_TARGET}_SCRIPT)
        set(PROJECT_EXECUTABLE ${SIGHT_TARGET})

        if(${FWEXEC_REQUIRE_ADMIN})
            get_admin_request_string(ADMIN_REQUEST)
        endif()

        configure_file(
            ${FWCMAKE_RESOURCE_PATH}/build/windows/template_exe.bat.in
            ${CMAKE_CURRENT_BINARY_DIR}/${${SIGHT_TARGET}_SCRIPT} @ONLY
        )
        unset(ADMIN_REQUEST)
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIR)
        file(
            COPY ${CMAKE_CURRENT_BINARY_DIR}/${${SIGHT_TARGET}_SCRIPT}
            DESTINATION ${CMAKE_BINARY_DIR}/bin
            FILE_PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        )
    endif()

    install(TARGETS ${SIGHT_TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Adds project into folder exec
    set_target_properties(${SIGHT_TARGET} PROPERTIES FOLDER "exec")
endmacro()

# Generic operations for a test based on the CppUnit framework
macro(sight_generic_test SIGHT_TARGET)
    set(options)
    set(oneValueArgs REQUIRE_X)
    set(multiValueArgs)
    cmake_parse_arguments(FWCPPUNITTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
        set(${SIGHT_TARGET}_PCH_LIB $<TARGET_OBJECTS:${${SIGHT_TARGET}_PCH_TARGET}>)
    endif()

    string(REGEX REPLACE "_ui?t$" "" DIRNAME "${SIGHT_TARGET}")

    set(BASE_TARGET "${DIRNAME}")

    if(TARGET ${BASE_TARGET})
        get_target_property(TARGET_TYPE ${BASE_TARGET} SIGHT_TARGET_TYPE)
        if("${TARGET_TYPE}" STREQUAL "MODULE")
            string(REPLACE "_" "::" BASE_MODULE ${BASE_TARGET})

            # This variable is used in cppunit_main.cpp to automatically load the module of the test
            set(TESTED_MODULE "${PROJECT_NAME}::${BASE_MODULE}")
        endif()
    endif()

    configure_file(
        "${FWCMAKE_RESOURCE_PATH}/build/cppunit_main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/cppunit_main.cpp" IMMEDIATE
        @ONLY
    )

    add_executable(
        ${SIGHT_TARGET}
        ${FWCPPUNITTEST_UNPARSED_ARGUMENTS}
        ${${SIGHT_TARGET}_HEADERS}
        ${${SIGHT_TARGET}_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/src/cppunit_main.cpp
        ${${SIGHT_TARGET}_RC_FILES}
        ${${SIGHT_TARGET}_CMAKE_FILES}
        ${${SIGHT_TARGET}_PCH_LIB}
    )

    # Do it here because add ".bin" suffix change the ${SIGHT_TARGET} (!!!)
    if(UNIX)
        set(PROJECT_EXECUTABLE "${SIGHT_TARGET}.bin")
        string(TOLOWER "${SIGHT_TARGET}" SIGHT_TEST_SCRIPT)

        # On linux add ".bin" suffix, to distinguish executable (.bin) from launcher script(.sh).
        set_target_properties(${SIGHT_TARGET} PROPERTIES SUFFIX ".bin")
    else()
        set(PROJECT_EXECUTABLE "${SIGHT_TARGET}")
        string(TOLOWER "${SIGHT_TARGET}.bat" SIGHT_TEST_SCRIPT)
    endif()

    set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_TARGET_TYPE "TEST")

    configure_project(${SIGHT_TARGET})

    if(EXISTS "${PRJ_SOURCE_DIR}/ut/rc")
        set(TEST_RC_DIR "${PRJ_SOURCE_DIR}/ut/rc")
    elseif(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(TEST_RC_DIR "${PRJ_SOURCE_DIR}/rc")
    elseif(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(TEST_RC_DIR "${PRJ_SOURCE_DIR}/rc")
    endif()
    if(TEST_RC_DIR)
        set(${SIGHT_TARGET}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")

        create_resources_target(${SIGHT_TARGET} ${SIGHT_TARGET}_rc "${TEST_RC_DIR}" "${${SIGHT_TARGET}_RC_BUILD_DIR}")
        add_dependencies(${SIGHT_TARGET} ${SIGHT_TARGET}_rc)
    endif()

    # Configure launcher script
    if(UNIX)
        # Use the right path separator on unix
        if(SIGHT_EXTERNAL_LIBRARIES)
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${SIGHT_EXTERNAL_LIBRARIES}/lib")
        else()
            string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
        endif()

        #fill the external modules
        set(SIGHT_EXTRA_MODULES_OPT "")
        foreach(MODULE ${SIGHT_EXTRA_MODULES})
            list(APPEND SIGHT_EXTRA_MODULES_OPT "-B \"${MODULE}\"")
        endforeach()

        # Build the shell script from template_test.sh.in
        configure_file(
            ${FWCMAKE_RESOURCE_PATH}/build/linux/template_test.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT}
            @ONLY
        )

        # Cleanup
        unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIR)
    else()
        # Build the bat script from template_test.bat.in
        set(SIGHT_EXTRA_MODULES_OPT "")
        foreach(MODULE ${SIGHT_EXTRA_MODULES})
            list(APPEND SIGHT_EXTRA_MODULES_OPT "-B \"${MODULE}\"")
        endforeach()

        configure_file(
            ${FWCMAKE_RESOURCE_PATH}/build/windows/template_test.bat.in
            ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT} @ONLY
        )
    endif()

    # Copy launcher script
    file(
        COPY ${CMAKE_CURRENT_BINARY_DIR}/${SIGHT_TEST_SCRIPT}
        DESTINATION ${CMAKE_BINARY_DIR}/bin/
        FILE_PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            OWNER_EXECUTE
            GROUP_READ
            GROUP_EXECUTE
            WORLD_READ
            WORLD_EXECUTE
    )

    # Adds project into folder test
    set_target_properties(${SIGHT_TARGET} PROPERTIES FOLDER "test")

    if(SIGHT_ENABLE_PCH AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
        target_precompile_headers(${SIGHT_TARGET} REUSE_FROM ${${SIGHT_TARGET}_PCH_TARGET})
        if(SIGHT_VERBOSE_PCH)
            message(STATUS "Use ${${SIGHT_TARGET}_PCH_TARGET} precompiled header")
        endif()
        if(UNIX)
            # CMAKE_POSITION_INDEPENDENT_CODE sets "-fPIE" but we also needs the "-fPIC" used in the PCH
            target_compile_options(${SIGHT_TARGET} PRIVATE "-fPIC")
        endif()
    endif()
endmacro()

# Create a GUI test
macro(sight_gui_test SIGHT_TARGET)
    sight_generic_test(${SIGHT_TARGET} REQUIRE_X ON)

    # Set test command
    if(UNIX)
        set(SCRIPT_SUFFIX "sh")
    else()
        set(SCRIPT_SUFFIX "bat")
    endif()
    add_test(NAME "${SIGHT_TEST_SCRIPT}" COMMAND ${CMAKE_BINARY_DIR}/bin/exec_gui_tests.${SCRIPT_SUFFIX}
                                                 ${SIGHT_TEST_SCRIPT} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    unset(SCRIPT_SUFFIX)
    unset(SIGHT_TEST_SCRIPT)
endmacro()

# Create a unit test
macro(fw_test SIGHT_TARGET)
    set(options)
    set(oneValueArgs
        TYPE
        PCH
        START
        PRIORITY
        CONSOLE
        OBJECT_LIBRARY
        WARNINGS_AS_ERRORS
        UNIQUE
        FAST_DEBUG
        REQUIRE_X
    )
    set(multiValueArgs)
    cmake_parse_arguments(SIGHT_CPPUNIT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(EXISTS "${${SIGHT_TARGET}_DIR}/ui")
        list(FILTER ${SIGHT_TARGET}_HEADERS EXCLUDE REGEX "/ui/")
        list(FILTER ${SIGHT_TARGET}_SOURCES EXCLUDE REGEX "/ui/")
    endif()
    sight_generic_test(${SIGHT_TARGET} REQUIRE_X ${SIGHT_CPPUNIT_REQUIRE_X})

    # Set test command
    if(TESTS_XML_OUTPUT)
        add_test(NAME "${SIGHT_TEST_SCRIPT}" COMMAND "${CMAKE_BINARY_DIR}/bin/${SIGHT_TEST_SCRIPT} --xml"
                 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    else()
        add_test(NAME "${SIGHT_TEST_SCRIPT}" COMMAND "${CMAKE_BINARY_DIR}/bin/${SIGHT_TEST_SCRIPT}"
                 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
    unset(SIGHT_TEST_SCRIPT)
endmacro()

# Create a library target
macro(fw_lib SIGHT_TARGET OBJECT_LIBRARY)

    if(${OBJECT_LIBRARY})
        set(TARGET_OBJECT_LIB ${SIGHT_TARGET}_obj)
        set(TARGET_NAME ${TARGET_OBJECT_LIB})
    else()
        set(TARGET_NAME ${SIGHT_TARGET})
    endif()

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
        set(${TARGET_NAME}_PCH_LIB $<TARGET_OBJECTS:${${TARGET_NAME}_PCH_TARGET}>)
    endif()

    if(${OBJECT_LIBRARY})

        add_library(
            ${TARGET_OBJECT_LIB} OBJECT
            ${${SIGHT_TARGET}_HEADERS} ${${SIGHT_TARGET}_SOURCES} ${${SIGHT_TARGET}_RC_FILES}
            ${${SIGHT_TARGET}_CMAKE_FILES} $<BUILD_INTERFACE:${${SIGHT_TARGET}_PCH_LIB}>
        )

        add_library(
            ${SIGHT_TARGET} SHARED $<TARGET_OBJECTS:${TARGET_OBJECT_LIB}> $<BUILD_INTERFACE:${${TARGET_NAME}_PCH_LIB}>
        )

        target_include_directories(
            ${TARGET_OBJECT_LIB}
            PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include> $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/>
                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/__/>
        )
        target_include_directories(${SIGHT_TARGET} PUBLIC $<INSTALL_INTERFACE:include>)
        target_link_libraries(${SIGHT_TARGET} PUBLIC ${TARGET_OBJECT_LIB})
    else()
        add_library(
            ${SIGHT_TARGET} SHARED ${${SIGHT_TARGET}_HEADERS} ${${SIGHT_TARGET}_SOURCES} ${${SIGHT_TARGET}_RC_FILES}
                                   ${${SIGHT_TARGET}_CMAKE_FILES} $<BUILD_INTERFACE:${${SIGHT_TARGET}_PCH_LIB}>
        )
        target_include_directories(
            ${SIGHT_TARGET}
            PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/> $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/>
                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/__/> $<INSTALL_INTERFACE:include>
        )
    endif()

    set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_TARGET_TYPE "LIBRARY")

    configure_project(${SIGHT_TARGET})

    # Set version properties
    set_target_properties(
        ${SIGHT_TARGET} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION
                                                              ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )

    set_target_properties(${SIGHT_TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_${SIGHT_TARGET})

    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        set(${SIGHT_TARGET}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
        create_resources_target(
            ${SIGHT_TARGET} ${SIGHT_TARGET}_rc "${PRJ_SOURCE_DIR}/rc" "${${SIGHT_TARGET}_RC_BUILD_DIR}"
        )
        add_dependencies(${SIGHT_TARGET} ${SIGHT_TARGET}_rc)

        create_resources_install_target("${${SIGHT_TARGET}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
        set_target_properties(
            ${SIGHT_TARGET} PROPERTIES SIGHT_MODULE_RC_DIR
                                       "\${_IMPORT_PREFIX}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}"
        )
    endif()

    # create the config.hpp for the current library
    get_header_file_install_destination()
    configure_header_file(${SIGHT_TARGET} "config.hpp" "${HEADER_FILE_DESTINATION}" "${HEADER_FILE_DESTINATION_REL}")

    # export and install target
    if(NOT ${SIGHT_TARGET} MATCHES "^pch.*")
        install(
            DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}/${HEADER_FILE_DESTINATION_REL}
            FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.hpp"
            PATTERN "*.hxx"
            PATTERN "*.cuh" # CUDA
            PATTERN "test/*" EXCLUDE
        )

        set(TARGETS_TO_EXPORT ${SIGHT_TARGET})

        if(${OBJECT_LIBRARY})
            set(TARGETS_TO_EXPORT ${SIGHT_TARGET} ${TARGET_OBJECT_LIB})
        endif()
        install(
            TARGETS ${TARGETS_TO_EXPORT}
            EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FW_INSTALL_PATH_SUFFIX}
        )

        if(WIN32)
            install(FILES $<TARGET_PDB_FILE:${SIGHT_TARGET}> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
        endif()

        # Add all targets to the build-tree export set
        export(EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
               FILE "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}_${SIGHT_TARGET}_Targets.cmake"
               NAMESPACE ${PROJECT_NAME}::
        )

        # Install sight_Project_Targets.cmake
        install(
            EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
            FILE ${PROJECT_NAME}_${SIGHT_TARGET}_Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}
        )
        get_property(COMPONENTS GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS)
        set_property(GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS ${COMPONENTS};${SIGHT_TARGET})

        # Add Sight targets dependencies
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake" "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                COPYONLY IMMEDIATE
            )
        else()
            # Create empty file
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake" "")
        endif()

        # Install the sight_project_Dependencies.cmake
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Dependencies.cmake"
                RENAME ${PROJECT_NAME}_${SIGHT_TARGET}_Dependencies.cmake DESTINATION ${FWCONFIG_PACKAGE_LOCATION}
        )

        # Adds project into folder lib
        set_target_properties(${SIGHT_TARGET} PROPERTIES FOLDER "lib")
        set_target_properties(${SIGHT_TARGET} PROPERTIES EXPORT_PROPERTIES "SIGHT_MODULE_RC_DIR")
    endif()

    if(SIGHT_ENABLE_PCH AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
        target_precompile_headers(${TARGET_NAME} REUSE_FROM ${${TARGET_NAME}_PCH_TARGET})
        if(SIGHT_VERBOSE_PCH)
            message(STATUS "Use ${${SIGHT_TARGET}_PCH_TARGET} precompiled header")
        endif()
    endif()

endmacro()

# Create a module target
# Disable too many statements error for cmake-lint
# cmake-lint: disable=R0915
macro(fw_module SIGHT_TARGET TARGET_TYPE TARGET_REQUIRE_ADMIN)

    if(SIGHT_ENABLE_PCH AND MSVC AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
        set(${SIGHT_TARGET}_PCH_LIB $<TARGET_OBJECTS:${${SIGHT_TARGET}_PCH_TARGET}>)
    endif()

    if(${SIGHT_TARGET}_SOURCES)

        add_library(
            ${SIGHT_TARGET} SHARED
            ${ARGN} ${${SIGHT_TARGET}_HEADERS} ${${SIGHT_TARGET}_SOURCES} ${${SIGHT_TARGET}_RC_FILES}
            ${${SIGHT_TARGET}_CMAKE_FILES} $<BUILD_INTERFACE:${${SIGHT_TARGET}_PCH_LIB}>
        )

        # create the custom command that may generate the plugin.xml and the registerServices.cpp file
        if(EXISTS "${${SIGHT_TARGET}_DIR}/rc/plugin.xml")
            target_sources(${SIGHT_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/registerServices.cpp")
            set_source_files_properties(
                "${CMAKE_CURRENT_BINARY_DIR}/registerServices.cpp" PROPERTIES GENERATED TRUE SKIP_AUTOMOC ON
            )

            plugin_setup("${SIGHT_TARGET}")
        endif()

        configure_project(${SIGHT_TARGET})

        set_target_properties(
            ${SIGHT_TARGET} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION
                                                                  ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

        # create the config.hpp for the current module
        get_header_file_install_destination()
        configure_header_file(
            ${SIGHT_TARGET} "config.hpp" "${HEADER_FILE_DESTINATION}" "${HEADER_FILE_DESTINATION_REL}"
        )

        # Allows include of type <ui/config.hpp>
        target_include_directories(${SIGHT_TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
        # Allows include of all folders in libs, i.e. <ui/..> <io/..> ...
        target_include_directories(${SIGHT_TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs>)
        # Allows include of type <__/..> <data/..> ...
        target_include_directories(${SIGHT_TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/__>)
        # Allows include of type <modules/../..>
        target_include_directories(${SIGHT_TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>)

        if(SIGHT_ENABLE_PCH AND NOT ${SIGHT_TARGET}_DISABLE_PCH)
            target_precompile_headers(${SIGHT_TARGET} REUSE_FROM ${${SIGHT_TARGET}_PCH_TARGET})
            if(SIGHT_VERBOSE_PCH)
                message(STATUS "Use ${${SIGHT_TARGET}_PCH_TARGET} precompiled header")
            endif()
        endif()
    else()
        add_library(${SIGHT_TARGET} INTERFACE ${${SIGHT_TARGET}_RC_FILES} ${${SIGHT_TARGET}_CMAKE_FILES})
    endif()

    if("${TARGET_TYPE}" STREQUAL "APP")
        set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_TARGET_TYPE "APP")
        set_target_properties(${SIGHT_TARGET} PROPERTIES FOLDER "app")

        # Install library if the app has one
        get_target_property(TARGET_TYPE ${SIGHT_TARGET} TYPE)
        if(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
            install(TARGETS ${SIGHT_TARGET} RUNTIME DESTINATION ${SIGHT_MODULE_LIB_PREFIX})
        endif()

        if(UNIX)
            # Install shortcut
            string(TOLOWER ${SIGHT_TARGET} APP_NAME)

            set(LAUNCHER "sightrun.bin")
            set(PROFILE_PATH "${SIGHT_TARGET}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}")
            else()
                set(LAUNCHER_PATH "$me")
            endif()

            # Configure launcher script
            # Replace all ';' path separator to unix style path separator ':'
            if(SIGHT_EXTERNAL_LIBRARIES)
                string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${SIGHT_EXTERNAL_LIBRARIES}/lib")
            else()
                string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIR "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
            endif()

            foreach(MODULE ${SIGHT_EXTRA_MODULES})
                list(APPEND SIGHT_EXTRA_MODULES_OPT "-B \"${MODULE}\"")
            endforeach()
            configure_file(
                ${FWCMAKE_RESOURCE_PATH}/build/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY
            )
            unset(FW_SIGHT_EXTERNAL_LIBRARIES_DIR)

            file(
                COPY ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}
                DESTINATION ${CMAKE_BINARY_DIR}/bin
                FILE_PERMISSIONS
                    OWNER_READ
                    OWNER_WRITE
                    OWNER_EXECUTE
                    GROUP_READ
                    GROUP_EXECUTE
                    WORLD_READ
                    WORLD_EXECUTE
            )

            add_man_page(${SIGHT_TARGET})

        elseif(WIN32)
            # Install shortcut
            string(TOLOWER ${SIGHT_TARGET} APP_NAME)

            set(LAUNCHER "sightrun.exe")
            set(PROFILE_PATH "${SIGHT_TARGET}/profile.xml")
            if(FW_BUILD_EXTERNAL)
                set(LAUNCHER_PATH "${Sight_BINARY_DIR}\\${LAUNCHER}")
                cmake_path(NORMAL_PATH LAUNCHER_PATH)
            else()
                set(LAUNCHER_PATH "%BINDIR%\\${LAUNCHER}")
            endif()
            file(TO_NATIVE_PATH "${PROFILE_PATH}" PROFILE_PATH)

            if(${TARGET_REQUIRE_ADMIN})
                get_admin_request_string(ADMIN_REQUEST)
            endif()

            foreach(MODULE ${SIGHT_EXTRA_MODULES})
                list(APPEND SIGHT_EXTRA_MODULES_OPT "-B \"${MODULE}\"")
            endforeach()
            configure_file(
                ${FWCMAKE_RESOURCE_PATH}/build/windows/template.bat.in ${CMAKE_BINARY_DIR}/bin/${APP_NAME}.bat @ONLY
            )

            # For the install, we use relative paths to the module directories
            set(SIGHT_EXTRA_MODULES_OPT "")
            foreach(MODULE ${SIGHT_EXTRA_MODULES})
                cmake_path(GET MODULE FILENAME MODULE_NAME)
                list(APPEND SIGHT_EXTRA_MODULES_OPT "-B \"../share/${MODULE_NAME}\"")
            endforeach()

            configure_file(
                ${FWCMAKE_RESOURCE_PATH}/install/windows/template.bat.in
                ${CMAKE_CURRENT_BINARY_DIR}/install/${APP_NAME}.bat @ONLY
            )
            install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/install/${APP_NAME}.bat DESTINATION bin)

            unset(ADMIN_REQUEST)
        endif()
    else()
        set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_TARGET_TYPE "MODULE")
        set_target_properties(${SIGHT_TARGET} PROPERTIES FOLDER "module")

        # Only prefix with the repository name for modules
        set_target_properties(${SIGHT_TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_${SIGHT_TARGET})

        install(TARGETS ${SIGHT_TARGET} EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
                RUNTIME DESTINATION ${SIGHT_MODULE_LIB_PREFIX}
        )

        # Add all targets to the build-tree export set
        export(EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
               FILE "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}_${SIGHT_TARGET}_Targets.cmake"
               NAMESPACE ${PROJECT_NAME}::
        )

        # Install sight_Project_Targets.cmake
        install(
            EXPORT ${PROJECT_NAME}_${SIGHT_TARGET}_Targets
            FILE ${PROJECT_NAME}_${SIGHT_TARGET}_Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${FWCONFIG_PACKAGE_LOCATION}
        )
        get_property(COMPONENTS GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS)
        set_property(GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS ${COMPONENTS};${SIGHT_TARGET})
    endif()

    set(${SIGHT_TARGET}_RC_BUILD_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
    if(EXISTS "${PRJ_SOURCE_DIR}/rc")
        create_resources_target(
            ${SIGHT_TARGET} ${SIGHT_TARGET}_rc "${PRJ_SOURCE_DIR}/rc" "${${SIGHT_TARGET}_RC_BUILD_DIR}"
        )
        add_dependencies(${SIGHT_TARGET} ${SIGHT_TARGET}_rc)
    endif()

    create_resources_install_target("${${SIGHT_TARGET}_RC_BUILD_DIR}" "${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")
    set_target_properties(
        ${SIGHT_TARGET} PROPERTIES SIGHT_MODULE_RC_DIR "\${_IMPORT_PREFIX}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}"
    )

    # Create a property SIGHT_MODULE_ID that can be used to generate profile files
    get_filename_component(SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REPLACE "${SOURCE_DIR_NAME}" "" MODULE_NAME ${SIGHT_TARGET})
    if(MODULE_NAME)
        string(REPLACE "_" "::" MODULE_NAME ${MODULE_NAME})
        # Strip trailing __ for anonymous modules such as sight::ui
        string(REPLACE "__" "" SOURCE_DIR_NAME ${SOURCE_DIR_NAME})
    endif()

    set(MODULE_ID "${MODULE_NAME}${SOURCE_DIR_NAME}")
    if(NOT "${TARGET_TYPE}" STREQUAL "APP")
        set(MODULE_ID "${PROJECT_NAME}::${MODULE_ID}")
    endif()
    set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_MODULE_ID "${MODULE_ID}")

    set_target_properties(
        ${SIGHT_TARGET}
        PROPERTIES EXPORT_PROPERTIES
                   "SIGHT_TARGET_TYPE;SIGHT_START;SIGHT_MODULE_RC_DIR;SIGHT_MODULE_DEPENDENCIES;SIGHT_MODULE_ID"
    )
endmacro()

# Return the precompiled header target from a target
function(
    get_pch_target
    TARGET
    TARGET_DIR
    TYPE
    PCH
    OBJECT_LIBRARY
    FAST_DEBUG
)

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
        if(FAST_DEBUG AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(PCH_SUFFIX "_og")
        endif()
        set(${TARGET_NAME}_PCH_TARGET pch_core${PCH_SUFFIX} PARENT_SCOPE)
    endif()

endfunction()

# Add a "Sight" target
# Disable too many branches error for cmake-lint
# cmake-lint: disable=R0912
macro(sight_add_target)

    set(options)
    set(oneValueArgs
        TYPE
        PCH
        CONSOLE
        OBJECT_LIBRARY
        WARNINGS_AS_ERRORS
        UNIQUE
        FAST_DEBUG
        REQUIRE_X
        REQUIRE_ADMIN
    )
    set(multiValueArgs)
    cmake_parse_arguments(SIGHT_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(NAME ${ARGV0})

    message(STATUS "Configuring ${NAME}: ${CMAKE_CURRENT_SOURCE_DIR}")

    init_project(${NAME} ${SIGHT_TARGET_TYPE})

    if(NOT DEFINED SIGHT_TARGET_PCH)
        set(SIGHT_TARGET_PCH ON)
    elseif(NOT SIGHT_TARGET_PCH)
        set(${SIGHT_TARGET}_DISABLE_PCH ON)
    endif()

    if(NOT DEFINED SIGHT_TARGET_OBJECT_LIBRARY)
        set(SIGHT_TARGET_OBJECT_LIBRARY OFF)
    endif()

    if(NOT DEFINED SIGHT_TARGET_FAST_DEBUG)
        set(SIGHT_TARGET_FAST_DEBUG OFF)
    endif()

    # Get the pch target, test the existence of type variable to exclude unbuilt projects
    if(SIGHT_ENABLE_PCH AND SIGHT_TARGET_TYPE AND SIGHT_TARGET_PCH)
        get_pch_target(
            ${NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${SIGHT_TARGET_TYPE} ${SIGHT_TARGET_PCH} ${SIGHT_TARGET_OBJECT_LIBRARY}
            ${SIGHT_TARGET_FAST_DEBUG}
        )
    endif()

    if("${SIGHT_TARGET_TYPE}" STREQUAL "EXECUTABLE")
        # If the user asked that the executable must be executed as an admin user, we forward the request
        if(SIGHT_TARGET_REQUIRE_ADMIN)
            fw_exec(${SIGHT_TARGET} CONSOLE ${SIGHT_TARGET_CONSOLE} REQUIRE_ADMIN ${SIGHT_TARGET_REQUIRE_ADMIN})
        else()
            fw_exec(${SIGHT_TARGET} CONSOLE ${SIGHT_TARGET_CONSOLE})
        endif()
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "LIBRARY")
        fw_lib(${SIGHT_TARGET} ${SIGHT_TARGET_OBJECT_LIBRARY})
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "MODULE")
        fw_module(${SIGHT_TARGET} ${SIGHT_TARGET_TYPE} OFF)
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "TEST")
        fw_test(${SIGHT_TARGET} REQUIRE_X ${SIGHT_TARGET_REQUIRE_X} "${OPTIONS}")
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "GUI_TEST")
        sight_gui_test(${SIGHT_TARGET} "${OPTIONS}")
    elseif("${SIGHT_TARGET_TYPE}" STREQUAL "APP")
        get_last_git_tag(${SIGHT_TARGET})
        set(${SIGHT_TARGET}_VERSION ${GIT_TAG})
        if(${SIGHT_TARGET_REQUIRE_ADMIN})
            fw_module(${SIGHT_TARGET} ${SIGHT_TARGET_TYPE} ON)
        else()
            fw_module(${SIGHT_TARGET} ${SIGHT_TARGET_TYPE} OFF)
        endif()
        if(NOT SIGHT_TARGET_UNIQUE)
            set(SIGHT_TARGET_UNIQUE "false")
        else()
            set(SIGHT_TARGET_UNIQUE "true")
        endif()
        set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_PROJECT_VERSION "${${SIGHT_TARGET}_VERSION}")
        set_target_properties(${SIGHT_TARGET} PROPERTIES SIGHT_UNIQUE "${SIGHT_TARGET_UNIQUE}")
    endif()

    if(NOT DEFINED SIGHT_TARGET_WARNINGS_AS_ERRORS OR SIGHT_TARGET_WARNINGS_AS_ERRORS)
        get_target_property(TARGET_TYPE ${SIGHT_TARGET} TYPE)
        # Skip libraries without code
        if(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
            fw_manage_warnings(${SIGHT_TARGET})
        endif()

        # Forward the flag on the object library if it is used
        if(SIGHT_TARGET_OBJECT_LIBRARY)
            fw_manage_warnings(${SIGHT_TARGET}_obj)
        endif()
    endif()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        if(SIGHT_TARGET_FAST_DEBUG)
            if(UNIX)
                target_compile_options(${SIGHT_TARGET} PRIVATE "-Og")
            elseif(MSVC)
                set_fast_debug_cxx_flags(${SIGHT_TARGET} ${SIGHT_TARGET_OBJECT_LIBRARY})
            endif()
        elseif(MSVC)
            restore_cxx_flags(${SIGHT_TARGET} ${SIGHT_CMAKE_CXX_FLAGS_DEBUG} ${SIGHT_TARGET_OBJECT_LIBRARY})
        endif()
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake")
        include("Dependencies.cmake")
    endif()

    # Generate batch script to ease the set of PATH in order to launch a Sight application on Windows.
    if(WIN32)
        configure_file(
            ${FWCMAKE_RESOURCE_PATH}/install/windows/setpath.bat.in ${CMAKE_BINARY_DIR}/bin/setpath.bat @ONLY
        )
    endif()

    if("${SIGHT_TARGET_TYPE}" STREQUAL "EXECUTABLE" OR "${SIGHT_TARGET_TYPE}" STREQUAL "APP")
        generic_install()
    endif()

    # Add the target to a global property, so we can perform other actions on it later
    get_property(sight_targets GLOBAL PROPERTY sight_targets)
    list(APPEND sight_targets ${SIGHT_TARGET})
    set_property(GLOBAL PROPERTY sight_targets "${sight_targets}")
endmacro()

# Used to generate "profile.xml", now done in sight_create_package_targets()
macro(sight_generate_profile TARGET)
    message(WARNING "sight_generate_profile is now deprecated, it is no longer required.")
endmacro()

# Treat warnings as errors if requested
#   to activate "warning as errors", simply write in the Properties.cmake of your project:
#   set(WARNINGS_AS_ERRORS ON)
macro(fw_manage_warnings PROJECT)
    if(MSVC)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
            # wd4996: deprecated declaration will be displayed as warning and not errors
            target_compile_options(${PROJECT} PRIVATE /WX /wd4996)
        else()
            message(WARNING "Your version of MSVC is too old to use WARNINGS_AS_ERRORS.")
        endif()
    endif()

    # deprecated declaration will be displayed as warning and not errors
    target_compile_options(
        ${PROJECT} PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang>:-Werror;-Wno-error=deprecated-declarations>"
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12
       AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13
    )
        # disable specific buggy warnings with GCC12, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105329
        target_compile_options(
            ${PROJECT}
            PRIVATE
                "$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-restrict;-Wno-stringop-overflow;-Wno-array-bounds>"
        )
    endif()
endmacro()

# Find link and manual dependencies for a target
# All targets that do not appear in FILTERED_COMPONENTS is excluded
function(find_target_dependencies TARGET TARGETS_FILTER RESULT_VAR)
    set(DEPENDENCY_LIST)
    set(RESULT "")
    list(APPEND DEPENDENCY_LIST ${TARGET})

    while(DEPENDENCY_LIST)

        list(POP_BACK DEPENDENCY_LIST DEPENDENCY)

        if(NOT PROCESSED_${DEPENDENCY})
            get_target_property(DEPENDS ${DEPENDENCY} LINK_LIBRARIES)
            get_target_property(INTERFACE_DEPENDS ${DEPENDENCY} INTERFACE_LINK_LIBRARIES)
            list(APPEND DEPENDS ${INTERFACE_DEPENDS})
            set(DEPENDS_COPY ${DEPENDS})
            foreach(dep ${DEPENDS})
                if(NOT ${dep} IN_LIST TARGETS_FILTER AND NOT "${dep}" MATCHES "_obj")
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
                if(NOT ${dep} IN_LIST TARGETS_FILTER AND NOT "${dep}" STREQUAL "sightrun" AND NOT "${dep}" STREQUAL
                                                                                              "sightlog"
                )
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
            add_custom_target(
                ${COMPONENT}_install ${CMAKE_COMMAND} -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -P
                                     ${COMPONENT_BINARY_DIR}/cmake_install.cmake
                COMMENT "Install component ${COMPONENT}..."
            )
            add_dependencies(${COMPONENT}_install ${COMPONENT})
        endif()
        get_target_property(DEPENDENCIES ${COMPONENT} MANUALLY_ADDED_DEPENDENCIES)
        list(FILTER DEPENDENCIES EXCLUDE REGEX "_rc")
        list(TRANSFORM DEPENDENCIES PREPEND ${PROJECT_NAME}::)
        set_target_properties(${COMPONENT} PROPERTIES SIGHT_MODULE_DEPENDENCIES "${DEPENDENCIES}")
    endforeach()

    if(WIN32)
        # Might be needed later to get ${OGRE_PLUGIN_DIR}
        find_package(OGRE REQUIRED)
    endif()

    get_property(SIGHT_APPS GLOBAL PROPERTY SIGHT_APPS)
    foreach(APP ${SIGHT_APPS})
        get_target_property(APP_BINARY_DIR ${APP} BINARY_DIR)

        # Add an install target for every app
        add_custom_target(
            ${APP}_install ${CMAKE_COMMAND} -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -P ${APP_BINARY_DIR}/cmake_install.cmake
            COMMENT "Install application ${APP}..."
        )
        add_dependencies(${APP}_install ${APP})

        # Compute all dependencies to find the imported components we need to copy
        set(IMPORTED_RC_DIRS "")
        set(IMPORTED_LIBS "")

        find_target_dependencies(${APP} "${SIGHT_COMPONENTS};${SIGHT_IMPORTED_COMPONENTS}" ALL_DEPENDS)

        profile_setup("${APP}" "${ALL_DEPENDS}")

        foreach(DEP ${ALL_DEPENDS})
            if(NOT ${DEP} IN_LIST SIGHT_COMPONENTS)
                list(APPEND IMPORTED_DEPENDS ${DEP})
            endif()
        endforeach()
        foreach(DEP ${IMPORTED_DEPENDS})
            get_target_property(CONFIG ${DEP} IMPORTED_CONFIGURATIONS)
            get_target_property(LIB ${DEP} IMPORTED_LOCATION_${CONFIG})
            if(LIB)
                list(APPEND IMPORTED_LIBS ${LIB})
            endif()

            get_target_property(RC_DIR ${DEP} SIGHT_MODULE_RC_DIR)
            if(RC_DIR)
                string(REGEX REPLACE "(.*)::.*" "\\1" FOLDER ${DEP})
                list(APPEND IMPORTED_RC_DIRS ${FOLDER},${RC_DIR})
            endif()
        endforeach()

        list(REMOVE_DUPLICATES IMPORTED_RC_DIRS)
        list(REMOVE_DUPLICATES IMPORTED_LIBS)

        if(IMPORTED_RC_DIRS OR IMPORTED_LIBS)
            configure_file(
                ${FWCMAKE_RESOURCE_PATH}/install/install_imported.cmake.in ${APP_BINARY_DIR}/install_imported.cmake
                @ONLY
            )

            # Add an install target that installs imported components
            add_custom_target(
                ${APP}_install_imported ${CMAKE_COMMAND} -P ${APP_BINARY_DIR}/install_imported.cmake
                COMMENT "Install imported targets..."
            )
            add_dependencies(${APP}_install ${APP}_install_imported)
        endif()

        # Add a dependency on each install target of dependencies found in this project
        find_target_dependencies(${APP} "${SIGHT_COMPONENTS}" DEPENDS)
        foreach(DEP ${DEPENDS})
            if(NOT ${DEP} MATCHES "_obj")
                add_dependencies(${APP}_install ${DEP}_install)
            endif()
        endforeach()

        # Add a fixup target for every app
        if(WIN32)

            # Determine if we need to copy Qml plugins
            find_target_dependencies(${APP} "${SIGHT_COMPONENTS};${SIGHT_IMPORTED_COMPONENTS}" DEPENDS_FOR_QML)
            foreach(DEP ${DEPENDS_FOR_QML})
                get_target_property(LINKED_DEPENDS ${DEP} LINK_LIBRARIES)
                if("${DEP}" MATCHES "sight::module_ui_qt" OR "${LINKED_DEPENDS}" MATCHES "Qml")
                    set(QML_SOURCE_DIR "${Qt5_DIR}/../../..$<$<CONFIG:Debug>:/debug>/qml")
                    break()
                endif()
            endforeach()

            list(APPEND DEPENDS ${IMPORTED_DEPENDS})
            list(REMOVE_DUPLICATES DEPENDS)
            add_custom_target(
                ${APP}_install_plugins
                ${CMAKE_COMMAND}
                -DDEPENDS="${DEPENDS}"
                -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DQT_PLUGINS_SOURCE_DIR="${Qt5_DIR}/../../..$<$<CONFIG:Debug>:/debug>/plugins"
                -DQML_SOURCE_DIR="${QML_SOURCE_DIR}"
                -DQT_DESTINATION="${CMAKE_INSTALL_BINDIR}/.."
                -DOGRE_PLUGIN_DIR="${OGRE_PLUGIN_DIR}"
                -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
                -P
                "${FWCMAKE_RESOURCE_PATH}/install/windows/install_plugins.cmake"
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

            configure_file(
                ${FWCMAKE_RESOURCE_PATH}/install/windows/windows_fixup.cmake.in ${CMAKE_BINARY_DIR}/windows_fixup.cmake
                @ONLY
            )
            add_custom_target(
                ${APP}_fixup ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/windows_fixup.cmake
                COMMENT "Fixup before packaging..."
            )
            add_dependencies(${APP}_fixup ${APP}_install ${APP}_install_plugins)
        endif()

        # Add a package target for every app
        add_custom_target(
            ${APP}_package ${CMAKE_CPACK_COMMAND} --config ${APP_BINARY_DIR}/CPackConfig.cmake
            COMMENT "Package application ${APP}..."
        )
        add_dependencies(${APP}_package ${APP}_install)
        if(WIN32)
            add_dependencies(${APP}_package ${APP}_fixup)
        endif()

    endforeach()

    get_last_git_tag("")
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

    # look inside install dir for packaging
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.")

    set(CPACK_PACKAGE_FILE_NAME "sight-${GIT_TAG}-${PLATFORM_SUFFIX}")

    set(CPACK_PACKAGE_VENDOR "IRCAD")
    set(CPACK_PACKAGE_NAME "Sight")
    set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake")
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake")
    include(CPack)

endfunction()

# Order sight component list, according to build dependency tree
function(order_components SIGHT_UNORDERED_COMPONENTS SIGHT_ORDERED_COMPONENTS)
    set(unordered_components ${SIGHT_UNORDERED_COMPONENTS})
    set(ordered_components)

    while(unordered_components)
        foreach(component ${unordered_components})
            set(resolved TRUE)

            # Find all component dependencies
            find_target_dependencies(${component} "${SIGHT_UNORDERED_COMPONENTS}" dependencies)

            # If we have dependencies, we check if all of them are not already in the ordered list
            if(dependencies)
                foreach(dependency ${dependencies})
                    if(NOT ${dependency} IN_LIST ordered_components AND NOT "${dependency}" MATCHES "_obj")
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

# Check if something links against a module
function(sight_forbid_module_link _targets)
    foreach(_target ${_targets})
        message(VERBOSE "Checking if '${_target}' is linked against a module")
        get_target_property(_depends ${_target} LINK_LIBRARIES)
        foreach(_depend ${_depends})
            if(TARGET ${_depend})
                get_target_property(_type ${_depend} SIGHT_TARGET_TYPE)
                if("${_type}" STREQUAL "MODULE")
                    string(APPEND _modules " ${_depend}")
                endif()
            endif()
        endforeach()
        if(_modules)
            message(FATAL_ERROR "${_target} can not link with module(s): ${_modules}")
        endif()
    endforeach()
endfunction()

# Generates ordered component list of current project by order of dependency (no dependency first)
# Print as STATUS ordered list of components.
# Export in PARENT_SCOPE COMPONENTS variable.
function(sight_generate_component_list COMPONENTS)
    get_property(UNORDERED_COMPONENTS GLOBAL PROPERTY ${PROJECT_NAME}_COMPONENTS)

    # Use the ordered list of components
    order_components("${UNORDERED_COMPONENTS}" SIGHT_ORDERED_COMPONENTS)
    message(STATUS "${PROJECT_NAME} component list: ${SIGHT_ORDERED_COMPONENTS}")
    set(COMPONENTS "${SIGHT_ORDERED_COMPONENTS}" PARENT_SCOPE)

    # Check if something links with a module, which is forbidden by design
    get_property(sight_targets GLOBAL PROPERTY sight_targets)
    sight_forbid_module_link("${sight_targets}")
endfunction()

# Utility macro for sight_configure_pch: creates the pch targets and setup optimized debug options if needed
macro(sight_create_pch_target _og)
    set(SIGHT_PCH_HEADERS
        <boost/algorithm/string.hpp>
        <boost/foreach.hpp>
        <boost/function_types/function_arity.hpp>
        <boost/function_types/result_type.hpp>
        <boost/graph/named_graph.hpp>
        <boost/lexical_cast.hpp>
        <boost/mpl/accumulate.hpp>
        <boost/mpl/apply.hpp>
        <boost/mpl/back_inserter.hpp>
        <boost/mpl/copy.hpp>
        <boost/mpl/empty.hpp>
        <boost/mpl/eval_if.hpp>
        <boost/mpl/if.hpp>
        <boost/mpl/placeholders.hpp>
        <boost/mpl/push_front.hpp>
        <boost/mpl/transform.hpp>
        <boost/mpl/vector.hpp>
        <boost/multi_index/hashed_index.hpp>
        <boost/multi_index/random_access_index.hpp>
        <boost/multi_index_container.hpp>
        <boost/noncopyable.hpp>
        <boost/preprocessor/cat.hpp>
        <boost/preprocessor/comparison/equal.hpp>
        <boost/preprocessor/comparison/greater_equal.hpp>
        <boost/preprocessor/control/expr_if.hpp>
        <boost/preprocessor/control/if.hpp>
        <boost/preprocessor/facilities/empty.hpp>
        <boost/preprocessor/facilities/expand.hpp>
        <boost/preprocessor/facilities/is_empty_variadic.hpp>
        <boost/preprocessor/facilities/overload.hpp>
        <boost/preprocessor/variadic/elem.hpp>
        <boost/property_tree/ptree.hpp>
        <boost/range/combine.hpp>
        <boost/range/iterator_range_core.hpp>
        <boost/thread/locks.hpp>
        <boost/thread/mutex.hpp>
        <boost/thread/once.hpp>
        <boost/thread/shared_mutex.hpp>
        <boost/tokenizer.hpp>
        <boost/type_traits.hpp>
        <boost/unordered_map.hpp>
        <glm/glm.hpp>
        <algorithm>
        <any>
        <array>
        <cassert>
        <chrono>
        <cmath>
        <csignal>
        <cstdint>
        <cstring>
        <deque>
        <exception>
        <filesystem>
        <functional>
        <future>
        <iostream>
        <iterator>
        <limits>
        <list>
        <map>
        <memory>
        <queue>
        <ranges>
        <regex>
        <set>
        <sstream>
        <string>
        <thread>
        <typeinfo>
        <unordered_map>
        <unordered_set>
        <utility>
        <vector>
    )

    if(${_og})
        set(SIGHT_PCH_NAME "pch_core_og")
    else()
        set(SIGHT_PCH_NAME "pch_core")
    endif()

    add_library(${SIGHT_PCH_NAME} STATIC ${FWCMAKE_BUILD_FILES_DIR}/pch.cpp)

    if(${_og})
        # Set the debug optimized flags the same way it is done in sight_add_target
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            if(UNIX)
                target_compile_options(${SIGHT_PCH_NAME} PRIVATE "-Og")
            elseif(MSVC)
                set_fast_debug_cxx_flags(${SIGHT_PCH_NAME} OFF)
            endif()
        endif()
    else()
        # Ensure we use the same compile flags with the other targets
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND MSVC)
            restore_cxx_flags(${SIGHT_PCH_NAME} ${SIGHT_CMAKE_CXX_FLAGS_DEBUG} OFF)
        endif()
    endif()

    target_precompile_headers(${SIGHT_PCH_NAME} PUBLIC ${SIGHT_PCH_HEADERS})

    # CLang does not always support std::filesystem
    find_package(Filesystem REQUIRED)
    target_link_libraries(${SIGHT_PCH_NAME} PUBLIC std::filesystem)

    # Boost
    find_package(Boost QUIET COMPONENTS system date_time iostreams log log_setup REQUIRED)

    target_link_libraries(${SIGHT_PCH_NAME} PUBLIC Boost::date_time Boost::iostreams Boost::log Boost::log_setup)
    target_compile_definitions(${SIGHT_PCH_NAME} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)

    # Glm
    if(WIN32)
        target_link_libraries(${SIGHT_PCH_NAME} PRIVATE glm::glm)
    else()
        # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
        target_include_directories(${SIGHT_PCH_NAME} SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
    endif()
endmacro()

# Enable precompiled headers for the project
macro(sight_configure_pch)
    find_package(glm QUIET REQUIRED)

    if(SIGHT_ENABLE_PCH)
        sight_create_pch_target(FALSE)

        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            sight_create_pch_target(TRUE)
        endif()
    endif()
endmacro()
