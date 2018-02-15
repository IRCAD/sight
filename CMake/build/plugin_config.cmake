
function(plugin_setup PROJECT HEADERS_DEPENDS)

    # set variables used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})
    set(PROJECT_NAME ${PROJECT})

    # Filter bundles and app requirements from the requirement list
    foreach(CURRENT_REQUIREMENT ${${PROJECT}_REQUIREMENTS})
        # to only consider bundles and app
        get_target_property(TARGET_TYPE ${CURRENT_REQUIREMENT} TYPE)
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "BUNDLE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            list(APPEND PROJECT_REQUIREMENTS ${CURRENT_REQUIREMENT})
         endif()
    endforeach()

    # Add each bundle deps to the requirement list
    foreach(CURRENT_BUNDLE_DEPS ${${PROJECT}_BUNDLE_DEPENDENCIES})
        list(APPEND PROJECT_REQUIREMENTS ${CURRENT_BUNDLE_DEPS})
    endforeach()

    set(PLUGIN_CONFIG_COMMAND   ${CMAKE_COMMAND}
                                -DPROJECT="${PROJECT}"
                                -DPROJECT_VERSION="${${PROJECT}_VERSION}"
                                -DPROJECT_DIR="${${PROJECT}_DIR}"
                                -DPROJECT_REQUIREMENTS="${PROJECT_REQUIREMENTS}"
                                -DCMAKE_SCRIPTS_DIR="${CMAKE_SOURCE_DIR}/CMake/build"
                                -DPLUGIN_OUTPUT_PATH="${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${PROJECT}-${${PROJECT}_VERSION}"
                                -DREGISTERSERVICES_OUTPUT_PATH="${CMAKE_BINARY_DIR}/${PROJECT}"
                                -P "${FWCMAKE_RESOURCE_PATH}/build/plugin_config_command.cmake"
    )
    list(APPEND PLUGIN_CONFIG_COMMAND_DEPENDS "${FWCMAKE_RESOURCE_PATH}/build/plugin_config.cmake"
                                              "${FWCMAKE_RESOURCE_PATH}/build/plugin_config_command.cmake")

    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/${PROJECT}/registerServices.cpp"
        COMMAND ${PLUGIN_CONFIG_COMMAND}
        MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/CMake/build/registerServices.cpp.in"
        DEPENDS ${PLUGIN_CONFIG_COMMAND_DEPENDS} ${HEADERS_DEPENDS} "${CMAKE_SOURCE_DIR}/CMake/build/plugin.xml.in"
        COMMENT "Generating files plugin for ${PROJECT}"
        )

endfunction()
