# Compute project requirement and generate service registration code
function(plugin_setup PROJECT)

    # set variables used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})
    set(PROJECT_NAME ${PROJECT})

    # Filter modules and app requirements from the requirement list
    foreach(CURRENT_REQUIREMENT ${${PROJECT}_REQUIREMENTS})
        # to only consider modules and app
        get_target_property(TARGET_TYPE ${CURRENT_REQUIREMENT} SIGHT_TARGET_TYPE)
        if("${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "MODULE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            list(APPEND PROJECT_REQUIREMENTS ${CURRENT_REQUIREMENT})
        endif()
    endforeach()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rc/plugin.xml")
        set(PLUGIN_CONFIG_COMMAND
            ${CMAKE_COMMAND} -DPROJECT="${PROJECT}" -DPROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            -DCMAKE_SCRIPTS_DIR="${FWCMAKE_BUILD_FILES_DIR}"
            -DREGISTERSERVICE_OUTPUT_PATH="${CMAKE_CURRENT_BINARY_DIR}" -DPROJECT_NAME="${PROJECT_NAME}" -P
            "${FWCMAKE_RESOURCE_PATH}/build/plugin_config_command.cmake"
        )
        list(APPEND PLUGIN_CONFIG_COMMAND_DEPENDS "${FWCMAKE_RESOURCE_PATH}/build/plugin_config.cmake"
             "${FWCMAKE_RESOURCE_PATH}/build/plugin_config_command.cmake"
        )

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/registerServices.cpp"
            COMMAND ${PLUGIN_CONFIG_COMMAND}
            MAIN_DEPENDENCY "${FWCMAKE_BUILD_FILES_DIR}/registerServices.cpp.in"
            DEPENDS ${PLUGIN_CONFIG_COMMAND_DEPENDS} "${CMAKE_CURRENT_SOURCE_DIR}/rc/plugin.xml"
            COMMENT "Generating service registration file for ${PROJECT}"
        )

        # Determine if the plugin need to be started in the profile.xml
        file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/rc/plugin.xml" PLUGIN_CONTENT)
        foreach(LINE ${PLUGIN_CONTENT})
            if("${LINE}" MATCHES "<plugin.*")
                if("${LINE}" MATCHES "<plugin.*autostart=\"true\".*")
                    set_target_properties(${PROJECT} PROPERTIES SIGHT_START ON)
                endif()
                break() # Do not parse the rest of the file
            endif()
        endforeach()

    endif()

endfunction()
