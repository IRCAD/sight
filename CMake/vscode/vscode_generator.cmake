# Generates VS Code workspace for all specified projects.
# Works only with sight CMake script.
function(vscodeGenerator)

    unset(REPO_FOLDERS)
    foreach(REPOSITORY ${ADDITIONAL_REPOSITORIES_FILES})
        file(READ ${REPOSITORY} REPO_RAW_NAME)
        string(STRIP ${REPO_RAW_NAME} REPO_RAW_NAME)
        string(TOUPPER ${REPO_RAW_NAME} REPO_NAME)
        get_filename_component(REPO_DIR ${REPOSITORY} DIRECTORY)
        get_filename_component(REPO_DIR ${REPO_DIR} ABSOLUTE)

        if(${REPO_NAME} STREQUAL "SIGHT" OR BUILD_${REPO_NAME})
            string(CONCAT REPO_FOLDER
                "{\n"
                "    \"name\": \"${REPO_RAW_NAME}\",\n"
                "    \"path\": \"${REPO_DIR}\"\n"
                "},\n"
            )
        endif()
        string(APPEND REPO_FOLDERS ${REPO_FOLDER})
    endforeach()

    if(WIN32)
        set(DBG_TYPE "cppvsdbg")
        set(ENV_NAME "PATH")
        set(ENV_VALUE "${FW_EXTERNAL_LIBRARIES_DIR};%PATH%")
        set(EXTERNAL_CONSOLE "true")
    elseif(APPLE)
        set(DBG_TYPE "cppdbg")
        set(ENV_NAME "DYLD_FALLBACK_LIBRARY_PATH")
        string(REPLACE ";" ":" FW_EXTERNAL_LIBRARIES_DIRS "${FW_EXTERNAL_LIBRARIES_DIR}")
        set(ENV_VALUE "${FW_EXTERNAL_LIBRARIES_DIRS}:$DYLD_FALLBACK_LIBRARY_PATH")
        set(EXTERNAL_CONSOLE "false")
    else()
        set(DBG_TYPE "cppdbg")
        set(ENV_NAME "LD_LIBRARY_PATH")
        string(REPLACE ";" ":" FW_EXTERNAL_LIBRARIES_DIRS "${FW_EXTERNAL_LIBRARIES_DIR}")
        set(ENV_VALUE "${FW_EXTERNAL_LIBRARIES_DIRS}:$LD_LIBRARY_PATH")
        set(EXTERNAL_CONSOLE "false")
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
                "    \"externalConsole\": ${EXTERNAL_CONSOLE}\n"
                "},\n"
            )
        elseif( TYPE STREQUAL "APP")
            string(CONCAT CONFIG
                "{\n"
                "    \"name\": \"[app] ${PRJ_NAME}\",\n"
                "    \"type\": \"${DBG_TYPE}\",\n"
                "    \"request\": \"launch\",\n"
                "    \"program\": \"${CMAKE_BINARY_DIR}/bin/fwlauncher${CMAKE_EXECUTABLE_SUFFIX}\",\n"
                "    \"args\": [\"${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${PRJ_NAME}_FULLNAME}/profile.xml\"],\n"
                "    \"stopAtEntry\": false,\n"
                "    \"cwd\": \"${CMAKE_BINARY_DIR}\",\n"
                "    \"environment\": [{\n"
                "        \"name\": \"${ENV_NAME}\",\n"
                "        \"value\": \"${ENV_VALUE}\"\n"
                "    }],\n"
                "    \"externalConsole\": ${EXTERNAL_CONSOLE}\n"
                "},\n"
            )
        endif()
        string(APPEND CONFIGS_LIST ${CONFIG})
    endforeach()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/vscode/sight.code-workspace.in ${CMAKE_BINARY_DIR}/sight.code-workspace @ONLY)

endfunction()
