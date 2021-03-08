# Generates VS Code workspace for all specified projects.
# Works only with sight CMake script.
function(vscodeGenerator)

    unset(REPO_FOLDERS)

    if(WIN32)
        set(DBG_TYPE "cppvsdbg")
        set(ENV_NAME "PATH")
        set(ENV_VALUE "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR};%PATH%")
        set(EXTERNAL_CONSOLE "true")
        set(LAUNCHER "sightrun${CMAKE_EXECUTABLE_SUFFIX}")
    else()
        set(DBG_TYPE "cppdbg")
        set(ENV_NAME "LD_LIBRARY_PATH")
        string(REPLACE ";" ":" FW_SIGHT_EXTERNAL_LIBRARIES_DIRS "${FW_SIGHT_EXTERNAL_LIBRARIES_DIR}")
        set(ENV_VALUE "${FW_SIGHT_EXTERNAL_LIBRARIES_DIRS}:$LD_LIBRARY_PATH")
        set(EXTERNAL_CONSOLE "false")
        set(MI_MODE "\"MIMode\": \"gdb\",\n")
        set(LAUNCHER "sightrun.bin-${sightrun_VERSION}")
    endif()
    unset(CONFIG_LIST)
    foreach(PRJ_NAME ${ARGV})
        if(NOT TARGET ${PRJ_NAME})
            message("Project target ${PRJ_NAME} doesn't exist")
            continue()
        endif()
        message(STATUS "Generating ${PRJ_NAME} VSCode project")
        set(TYPE ${${PRJ_NAME}_TYPE})
        unset(CONFIG)
        if( TYPE STREQUAL "EXECUTABLE" )
            string(CONCAT CONFIG
                "{\n"
                "    \"name\": \"[exe] ${PRJ_NAME}\",\n"
                "    \"type\": \"${DBG_TYPE}\",\n"
                "    \"request\": \"launch\",\n"
                "    \"program\": \"${CMAKE_BINARY_DIR}/bin/${PRJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}\",\n"
                "    \"args\": [],\n"
                "    \"stopAtEntry\": false,\n"
                "    \"cwd\": \"${CMAKE_BINARY_DIR}\",\n"
                "    \"environment\": [{\n"
                "        \"name\": \"${ENV_NAME}\",\n"
                "        \"value\": \"${ENV_VALUE}\"\n"
                "    }],\n"
                "    ${MI_MODE}"
                "    \"externalConsole\": ${EXTERNAL_CONSOLE}\n"
                "},\n"
            )
        elseif( TYPE STREQUAL "TEST" )
            string(CONCAT CONFIG
                "{\n"
                "    \"name\": \"[test] ${PRJ_NAME}\",\n"
                "    \"type\": \"${DBG_TYPE}\",\n"
                "    \"request\": \"launch\",\n"
                "    \"program\": \"${CMAKE_BINARY_DIR}/bin/${PRJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}\",\n"
                "    \"args\": [],\n"
                "    \"stopAtEntry\": false,\n"
                "    \"cwd\": \"${CMAKE_BINARY_DIR}\",\n"
                "    \"environment\": [{\n"
                "        \"name\": \"${ENV_NAME}\",\n"
                "        \"value\": \"${ENV_VALUE}\"\n"
                "    }],\n"
                "    ${MI_MODE}"
                "    \"externalConsole\": ${EXTERNAL_CONSOLE}\n"
                "},\n"
            )
        elseif( TYPE STREQUAL "APP")
            string(CONCAT CONFIG
                "{\n"
                "    \"name\": \"[app] ${PRJ_NAME}\",\n"
                "    \"type\": \"${DBG_TYPE}\",\n"
                "    \"request\": \"launch\",\n"
                "    \"program\": \"${CMAKE_BINARY_DIR}/bin/${LAUNCHER}\",\n"
                "    \"args\": [\"${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${PRJ_NAME}_FULLNAME}/profile.xml\"],\n"
                "    \"stopAtEntry\": false,\n"
                "    \"cwd\": \"${CMAKE_BINARY_DIR}\",\n"
                "    \"environment\": [{\n"
                "        \"name\": \"${ENV_NAME}\",\n"
                "        \"value\": \"${ENV_VALUE}\"\n"
                "    }],\n"
                "    ${MI_MODE}"
                "    \"externalConsole\": ${EXTERNAL_CONSOLE}\n"
                "},\n"
            )
        endif()
        string(APPEND CONFIGS_LIST ${CONFIG})
    endforeach()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/vscode/sight.code-workspace.in ${CMAKE_BINARY_DIR}/sight.code-workspace @ONLY)

endfunction()
