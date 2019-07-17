#Windows install
macro(win_install PRJ_NAME)

    set(CPACK_GENERATOR NSIS)
    set(CPACK_SOURCE_GENERATOR ZIP)

    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        set(LAUNCHER "fwlauncher.exe")
        set(LAUNCHER_PATH "bin/${LAUNCHER}")
        set(PROFILE_PATH "${${PRJ_NAME}_BUNDLE_DIR}/profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")
        set(LAUNCHER_PATH "bin/${PRJ_NAME}.exe")
        set(PROFILE_PATH "")
    elseif()
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()

    list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/)

    #configure the 'fixup' script
    set(PROJECT_REQUIREMENTS ${${PROJECT}_REQUIREMENTS})

    if(${FW_BUILD_EXTERNAL})
        # install requirements
        findAllDependencies("${PROJECT}" PROJECT_LIST)

        # install requirements
        foreach(REQUIREMENT ${PROJECT_LIST})
            if(${REQUIREMENT}_EXTERNAL)
                # search and setup qt plugins for each bundles
                qt_plugins_setup(${REQUIREMENT})

                if(EXISTS "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION "${FWBUNDLE_LIB_PREFIX}")
                endif()
                if(EXISTS "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                    install(DIRECTORY "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION "${FWBUNDLE_RC_PREFIX}")
                endif()
            endif()
        endforeach()

        install_qt_plugins()
    endif()

    configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/windows_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/windows_fixup.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/windows_fixup.cmake)

    if(CMAKE_CL_64)
        set(CPACK_NSIS_INSTALL_DIR "$PROGRAMFILES64")
    endif()

    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

    set(CPACK_PACKAGE_VENDOR "IRCAD-IHU")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/IRCAD-IHU/sight")
    set(CPACK_NSIS_CONTACT "https://gitter.im/IRCAD-IHU/sight-support")

    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${VERSION}-win64-Sight_${GIT_TAG}")
    set(CPACK_NSIS_PACKAGE_NAME "${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
    set(CPACK_NSIS_DISPLAY_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_VERSION "${VERSION}")
    set(CPACK_BUNDLE_RC_PREFIX "${FWBUNDLE_RC_PREFIX}")

    set(DEFAULT_NSIS_RC_PATH "${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/rc/")

    # Clear variables otherwise they are not evaluated when we modify PROJECTS_TO_INSTALL
    unset(CPACK_PACKAGE_ICON CACHE)
    unset(CPACK_NSIS_WELCOMEFINISH_IMAGE CACHE)
    unset(CPACK_NSIS_MUI_ICON CACHE)
    unset(CPACK_NSIS_MUI_UNIICON CACHE)
    unset(CPACK_RESOURCE_FILE_LICENSE CACHE)

    find_file(CPACK_PACKAGE_ICON "banner_nsis.bmp" PATHS
                "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/" ${DEFAULT_NSIS_RC_PATH}
                NO_SYSTEM_ENVIRONMENT_PATH)
    find_file(CPACK_NSIS_WELCOMEFINISH_IMAGE "dialog_nsis.bmp" PATHS
                "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/" ${DEFAULT_NSIS_RC_PATH}
                NO_SYSTEM_ENVIRONMENT_PATH)
    find_file(CPACK_NSIS_MUI_ICON "${ICON_FILENAME}" "app.ico" PATHS
                "${CMAKE_CURRENT_SOURCE_DIR}/rc/" ${DEFAULT_NSIS_RC_PATH}
                NO_SYSTEM_ENVIRONMENT_PATH)
    find_file(CPACK_NSIS_MUI_UNIICON "${ICON_FILENAME}" "app.ico" PATHS
                "${CMAKE_CURRENT_SOURCE_DIR}/rc/" ${DEFAULT_NSIS_RC_PATH}
                NO_SYSTEM_ENVIRONMENT_PATH)
    find_file(CPACK_RESOURCE_FILE_LICENSE "license.rtf" PATHS
                "${CMAKE_CURRENT_SOURCE_DIR}/rc/NSIS/" ${DEFAULT_NSIS_RC_PATH}
                NO_SYSTEM_ENVIRONMENT_PATH)

    # Extract the icon found for the installer and use it for every shortcut (Start menu, Desktop and Uninstall)
    # The output variable is used in our NSIS.template
    string(REGEX REPLACE ".*\/(.*)" "\\1" CPACK_NSIS_SIGHT_APP_ICON ${CPACK_NSIS_MUI_ICON})

    string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_WELCOMEFINISH_IMAGE ${CPACK_NSIS_WELCOMEFINISH_IMAGE})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})
    string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})

    include(CPack)

endmacro()
