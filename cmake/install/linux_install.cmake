#Linux install
macro(linux_install PRJ_NAME)

    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    get_target_property(TARGET_TYPE ${FWPROJECT_NAME} SIGHT_TARGET_TYPE)

    if("$TARGET_TYPE}" STREQUAL  "APP")

        set(LAUNCHER_PATH "bin/sightrun.bin-${sightrun_VERSION}")
        set(LAUNCHER "sightrun.bin-${sightrun_VERSION}")
        set(PROFILE_PATH "${PRJ_NAME}-${PROJECT_VERSION}/profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${TARGET_TYPE}" STREQUAL  "EXECUTABLE")

        set(LAUNCHER_PATH "bin/${PRJ_NAME}.bin-${${PRJ_NAME}_VERSION}")
        set(LAUNCHER "${PRJ_NAME}.bin-${${PRJ_NAME}_VERSION}")
        set(PROFILE_PATH "")

    elseif()
        message(FATAL_ERROR "'${PRJ_NAME}' is not an installable (type : ${TARGET_TYPE})")
    endif()

    set(PROJECT_REQUIREMENTS ${${PROJECT}_REQUIREMENTS})

    if(${FW_BUILD_EXTERNAL})
        # install requirements
        findAllDependencies("${PROJECT}" PROJECT_LIST)

        foreach(REQUIREMENT ${PROJECT_LIST})
            if(${REQUIREMENT}_EXTERNAL)
                if(EXISTS "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${SIGHT_MODULE_LIB_PREFIX})
                endif()
                if(EXISTS "${Sight_MODULES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_MODULES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${SIGHT_MODULE_RC_PREFIX})
                endif()
            endif()
        endforeach()
    endif()

    if(${PRJ_NAME} STREQUAL "sight")
        # Needed for fixup_bundle first argument
        set(LAUNCHER_PATH "bin/sightrun.bin-${sightrun_VERSION}")
    endif()

    #configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
    #install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)

    if("${TARGET_TYPE}" STREQUAL  "APP")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

    elseif("${TARGET_TYPE}" STREQUAL  "EXECUTABLE")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template_exe.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif()

endmacro()
