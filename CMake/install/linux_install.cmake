#Linux install
macro(linux_install PRJ_NAME)

    set(CPACK_GENERATOR TGZ)
    set(CPACK_SOURCE_GENERATOR TGZ)
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")

        set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
        set(LAUNCHER "fwlauncher-${fwlauncher_VERSION}")
        set(PROFILE_PATH "${PRJ_NAME}-${PROJECT_VERSION}/profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")

        set(LAUNCHER_PATH "bin/${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(LAUNCHER "${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(PROFILE_PATH "")

    elseif()
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()

    set(PROJECT_REQUIREMENTS ${${PROJECT}_REQUIREMENTS})

    if(${FW_BUILD_EXTERNAL})
        # install requirements
        findAllDependencies("${PROJECT}" PROJECT_LIST)

        foreach(REQUIREMENT ${PROJECT_LIST})
            if(${REQUIREMENT}_EXTERNAL)
                # search and setup qt plugins for each bundles
                qt_plugins_setup(${REQUIREMENT})

                if(EXISTS "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${FWBUNDLE_LIB_PREFIX})
                endif()
                if(EXISTS "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${FWBUNDLE_RC_PREFIX})
                endif()
            endif()
        endforeach()

        install_qt_plugins()
    endif()

    if(${PRJ_NAME} STREQUAL "sight")
        # Needed for fixup_bundle first argument
        set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
    endif()

    configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)

    set(CPACK_OUTPUT_FILE_PREFIX packages)
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

    execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

    set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${VERSION}-linux_${ARCHITECTURE}-Sight_${GIT_TAG}")
    set(CPACK_PACKAGE_VENDOR "IRCAD-IHU")
    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_VERSION "${VERSION}")

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template_exe.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif()

    include(CPack)

endmacro()
