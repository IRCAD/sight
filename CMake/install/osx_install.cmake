function(osx_install PRJ_NAME)

    set(EXECUTABLE_NAME ${PRJ_NAME}.app)

    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.icns)

    set_source_files_properties(rc/${ICON_FILENAME} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        # create a new executable equivalent to fwLauncher
        add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${fwlauncher_HEADERS} ${fwlauncher_SOURCES} rc/${ICON_FILENAME})
        add_definitions(-DDEFAULT_PROFILE=${BUNDLE_RC_PREFIX}/${PRJ_NAME}-${${PRJ_NAME}_VERSION}/profile.xml)
        get_target_property(LAUNCHER_DEFINITIONS fwlauncher COMPILE_DEFINITIONS)
        target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ${LAUNCHER_DEFINITIONS})
        get_target_property(LINK_PROPERTIES fwlauncher LINK_LIBRARIES)
        target_link_libraries(${EXECUTABLE_NAME} ${LINK_PROPERTIES})
    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")
        add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${${PRJ_NAME}_HEADERS} ${${PRJ_NAME}_SOURCES} rc/${ICON_FILENAME})
        get_target_property(LAUNCHER_DEFINITIONS ${PRJ_NAME} COMPILE_DEFINITIONS)
        target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ${LAUNCHER_DEFINITIONS})
        get_target_property(LINK_PROPERTIES ${PRJ_NAME} LINK_LIBRARIES)
        target_link_libraries(${EXECUTABLE_NAME} ${LINK_PROPERTIES})
    else()
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()

    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME ${PRJ_NAME})
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_FILENAME})
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PRJ_NAME}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${${PRJ_NAME}_VERSION}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${${PRJ_NAME}_VERSION}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PRJ_NAME}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2017 IRCAD-IHU")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.fw4spl.${LOWER_PRJ_NAME}")

    set(APP_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE_NAME}")
    set(APP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}")

    # configure qt.conf
    set(FW_QT5_PLUGINS_PATH "lib/qt5/plugins")
    set(QT_DIR "./")
    configure_file("${fwGuiQt_DIR}/bin/qt.conf" "${APP_PATH}/Contents/Resources/qt.conf")

    install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION . COMPONENT ApplicationBundle)


    install(CODE "
        if (EXISTS \"${CMAKE_INSTALL_PREFIX}/${BUNDLE_RC_PREFIX}\")
            file(INSTALL \"${CMAKE_INSTALL_PREFIX}/${BUNDLE_RC_PREFIX}\" DESTINATION \"${APP_INSTALL_PATH}/Contents\")
        endif()
        if (EXISTS ${CMAKE_INSTALL_PREFIX}/${BUNDLE_LIB_PREFIX})
            file(INSTALL \"${CMAKE_INSTALL_PREFIX}/${BUNDLE_LIB_PREFIX}\" DESTINATION \"${APP_INSTALL_PATH}/Contents\")
        endif()
        if (EXISTS ${CMAKE_INSTALL_PREFIX}/lib/qt5/plugins)
            file(INSTALL \"${CMAKE_INSTALL_PREFIX}/lib/qt5/plugins\" DESTINATION \"${APP_INSTALL_PATH}/Contents/lib/qt5\")
        endif()

        # copy the other installed files or directories (not .app, lib, bin, share)
        file(GLOB FILES \"${CMAKE_INSTALL_PREFIX}/*\")

        foreach(FILE \${FILES})
            if (NOT \${FILE} MATCHES \"${EXECUTABLE_NAME}\"
                AND NOT \${FILE} MATCHES \"${BUNDLE_RC_PREFIX}\"
                AND NOT \${FILE} MATCHES \"${BUNDLE_LIB_PREFIX}\"
                AND NOT \${FILE} MATCHES \"${CMAKE_INSTALL_LIBDIR}\"
                AND NOT \${FILE} MATCHES \"bin\"
            )
                file(INSTALL \${FILE} DESTINATION \"${APP_INSTALL_PATH}/Contents\")
            endif()
        endforeach()
    " COMPONENT ApplicationBundle)

    install(CODE "
        file(GLOB_RECURSE QTPLUGINS \"${APP_INSTALL_PATH}/Contents/lib/qt5/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        file(GLOB_RECURSE BUNDLES \"${APP_INSTALL_PATH}/Contents/${BUNDLE_LIB_PREFIX}/*/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

        # Find Bundles for fixup
        if (BUNDLES)
            set (BUNDLES_TO_FIX )
            foreach(CURRENT_BUNDLE \${BUNDLES})
                if (NOT IS_SYMLINK \${CURRENT_BUNDLE})
                    list(APPEND BUNDLES_TO_FIX \${CURRENT_BUNDLE} )
                endif()
            endforeach()
            # Get Bundles folders
            set(BUNDLES_FOLDERS)
            foreach(BUNDLE \${BUNDLES_TO_FIX})
                get_filename_component(BUNDLE_DIR \"\${BUNDLE}\" DIRECTORY)
                list(APPEND BUNDLES_FOLDERS \${BUNDLE_DIR})
            endforeach()
            list(REMOVE_DUPLICATES BUNDLES_FOLDERS)
        endif()

        # If the BU_CHMOD_BUNDLE_ITEMS variable is set then bundle items will be marked writable before install_name_tool tries to change them.
        set(BU_CHMOD_BUNDLE_ITEMS ON)

        include(BundleUtilities)
        fixup_bundle(\"${APP_INSTALL_PATH}\" \"\${BUNDLES_TO_FIX};\${QTPLUGINS}\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR};${EXTERNAL_LIBRARIES}/lib;\${BUNDLES_FOLDERS}\")

        execute_process(
            COMMAND sh -c \"find . -type f -name '*.dylib'|sed 's/\\\\.[0-9].*//g'|sort|uniq -d|xargs -I{} -n1 find . -path '{}.*' | paste -d ' ' - -| sed 's/^/ln -s -f /' | sh \"
            WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents/MacOS
            )

        execute_process(
            COMMAND sh -c \"python ${CMAKE_SOURCE_DIR}/CMake/OSXTools/osx_install_name_tool.py -e MacOS/ $(find \"${BUNDLE_LIB_PREFIX}\" -iname '*.dylib') -f\"
            WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents
            )
    " COMPONENT ApplicationBundle)

endfunction(osx_install)
