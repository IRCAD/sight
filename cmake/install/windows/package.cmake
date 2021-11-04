#Windows install
macro(win_package PRJ_NAME)

    # Magic line to make CPack NSIS generator find NSIS.template.in and NSIS.InstallOptions.ini.in
    list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/)

    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    get_target_property(TYPE ${PRJ_NAME} SIGHT_TARGET_TYPE)
    if("${TYPE}" STREQUAL "APP")
        set(LAUNCHER "sightrun.exe")
        set(CPACK_LAUNCHER_PATH "bin\\\\${LAUNCHER}")
        set(CPACK_PROFILE_PATH "${SIGHT_MODULE_RC_PREFIX}\\\\${SIGHT_TARGET}\\\\profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${TYPE}" STREQUAL "EXECUTABLE")
        set(CPACK_LAUNCHER_PATH "bin\\\\${PRJ_NAME}.exe")
        set(CPACK_PROFILE_PATH "")
    elseif()
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${TYPE})")
    endif()

    if(CMAKE_CL_64)
        set(CPACK_NSIS_INSTALL_DIR "$PROGRAMFILES64")
    endif()

    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/IRCAD/sight")
    set(CPACK_NSIS_CONTACT "https://gitter.im/IRCAD-IHU/sight-support")

    get_platform_package_suffix()

    set(CPACK_GENERATOR NSIS)
    set(CPACK_SOURCE_GENERATOR ZIP)
    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${GIT_TAG}-${PLATFORM_SUFFIX}")
    set(CPACK_PACKAGE_VERSION "${SIGHT_VERSION}")
    set(CPACK_PACKAGE_VENDOR "IRCAD")
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${PRJ_NAME};ALL;.")

    # Look inside install dir for packaging
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.")

    set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake")
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake")
    set(CPACK_NSIS_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_NSIS_DISPLAY_NAME "${PRJ_NAME}")
    set(CPACK_SIGHT_MODULE_RC_PREFIX "${SIGHT_MODULE_RC_PREFIX}")

    # CPackComponent doesn't work properly with the 'single project' packaging mechanism we hacked here
    # As a workaround, this script allows to remove the dev component files before packing
    set(CPACK_PRE_BUILD_SCRIPTS ${FWCMAKE_RESOURCE_PATH}/install/pre_package.cmake)

    set(DEFAULT_NSIS_RC_PATH "${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/rc/")

    # Clear variables otherwise they are not evaluated when we modify PROJECTS_TO_INSTALL
    unset(CPACK_PACKAGE_ICON CACHE)
    unset(CPACK_NSIS_WELCOMEFINISH_IMAGE CACHE)
    unset(CPACK_NSIS_MUI_ICON CACHE)
    unset(CPACK_NSIS_MUI_UNIICON CACHE)
    unset(CPACK_RESOURCE_FILE_LICENSE CACHE)

    find_file(CPACK_PACKAGE_ICON "banner_nsis.bmp" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/" ${DEFAULT_NSIS_RC_PATH}
              NO_SYSTEM_ENVIRONMENT_PATH
    )
    find_file(CPACK_NSIS_WELCOMEFINISH_IMAGE "dialog_nsis.bmp" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/"
                                                                     ${DEFAULT_NSIS_RC_PATH} NO_SYSTEM_ENVIRONMENT_PATH
    )
    find_file(CPACK_NSIS_MUI_ICON "${ICON_FILENAME}" "app.ico" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/rc/"
                                                                     ${DEFAULT_NSIS_RC_PATH} NO_SYSTEM_ENVIRONMENT_PATH
    )
    find_file(CPACK_NSIS_MUI_UNIICON "${ICON_FILENAME}" "app.ico" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/rc/"
                                                                        ${DEFAULT_NSIS_RC_PATH}
              NO_SYSTEM_ENVIRONMENT_PATH
    )
    find_file(CPACK_RESOURCE_FILE_LICENSE "license.rtf" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/"
                                                              ${DEFAULT_NSIS_RC_PATH} NO_SYSTEM_ENVIRONMENT_PATH
    )

    # Extract the icon found for the installer and use it for every shortcut (Start menu, Desktop and Uninstall)
    # The output variable is used in our NSIS.template
    string(REGEX REPLACE ".*\/(.*)" "\\1" CPACK_NSIS_SIGHT_APP_ICON ${CPACK_NSIS_MUI_ICON})
    string(REPLACE "/" "\\\\" CPACK_SIGHT_MODULE_RC_PREFIX ${CPACK_SIGHT_MODULE_RC_PREFIX})

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_NSIS_SIGHT_APP_ICON}")
        install(FILES ${CPACK_NSIS_MUI_ICON} DESTINATION "${CPACK_SIGHT_MODULE_RC_PREFIX}/${CPACK_NSIS_PACKAGE_NAME}")
    endif()

    string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_WELCOMEFINISH_IMAGE ${CPACK_NSIS_WELCOMEFINISH_IMAGE})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})
    string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})

    # Avoid a useless warning
    include(CPack)

endmacro()
