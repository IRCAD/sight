function(osx_install PRJ_NAME)

    set(EXECUTABLE_NAME ${PRJ_NAME}.app)

    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    find_file(${PRJ_NAME}_ICON_PATH NAMES "${LOWER_PRJ_NAME}.icns" "app.icns" PATHS
              "${CMAKE_CURRENT_SOURCE_DIR}/rc/" ${FWCMAKE_RESOURCE_PATH}/install/macos/rc/
              NO_SYSTEM_ENVIRONMENT_PATH)

    get_filename_component(ICON_FILENAME ${${PRJ_NAME}_ICON_PATH} NAME)

    set_source_files_properties(${${PRJ_NAME}_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    if(TARGET fwlauncher)
        if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
            # create a new executable equivalent to fwLauncher
            add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${fwlauncher_HEADERS} ${fwlauncher_SOURCES} ${${PRJ_NAME}_ICON_PATH})
            add_definitions(-DDEFAULT_PROFILE=${FWBUNDLE_RC_PREFIX}/${PRJ_NAME}-${${PRJ_NAME}_VERSION}/profile.xml)
            get_target_property(LAUNCHER_DEFINITIONS fwlauncher COMPILE_DEFINITIONS)
            target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ${LAUNCHER_DEFINITIONS})
            get_target_property(LINK_PROPERTIES fwlauncher LINK_LIBRARIES)
            target_link_libraries(${EXECUTABLE_NAME} ${LINK_PROPERTIES})
            set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
        elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")
            message(WARNING "EXECUTABLE cannot be installed yet on macOS")
            return()
        else()
            message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
        endif()

        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME ${PRJ_NAME})
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_FILENAME})
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PRJ_NAME}")
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${${PRJ_NAME}_VERSION}")
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${${PRJ_NAME}_VERSION}")
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PRJ_NAME}")
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2018 IRCAD-IHU")
        set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.sight.${LOWER_PRJ_NAME}")

        set(APP_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE_NAME}")
        set(APP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}")

        # configure qt.conf
        install(FILES "${CMAKE_SOURCE_DIR}/CMake/install/macos/rc/qt.conf" DESTINATION "${APP_INSTALL_PATH}/Contents/Resources/" COMPONENT ApplicationBundle)

        install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION . COMPONENT ApplicationBundle)

        install(CODE "

            if (EXISTS \"${CMAKE_INSTALL_PREFIX}/${FWBUNDLE_RC_PREFIX}\")
                file(INSTALL \"${CMAKE_INSTALL_PREFIX}/${FWBUNDLE_RC_PREFIX}/\" DESTINATION \"${APP_INSTALL_PATH}/Contents/${FWBUNDLE_RC_PREFIX}\")
            endif()

            if (EXISTS ${CMAKE_INSTALL_PREFIX}/${FWBUNDLE_LIB_PREFIX})
                file(INSTALL \"${CMAKE_INSTALL_PREFIX}/${FWBUNDLE_LIB_PREFIX}/\" DESTINATION \"${APP_INSTALL_PATH}/Contents/${FWBUNDLE_LIB_PREFIX}\")
            endif()

            if (EXISTS ${CMAKE_INSTALL_PREFIX}/lib/qt5/plugins)
                file(INSTALL \"${CMAKE_INSTALL_PREFIX}/lib/qt5/plugins/\" DESTINATION \"${APP_INSTALL_PATH}/Contents/MacOS/qt5/plugins\")
            endif()

            if (EXISTS ${CMAKE_INSTALL_PREFIX}/qml)
                file(INSTALL \"${CMAKE_INSTALL_PREFIX}/qml/\" DESTINATION \"${APP_INSTALL_PATH}/Contents/qml\")
            endif()

            if (EXISTS ${CMAKE_INSTALL_PREFIX}/bin/Contents/Plugins)
                file(INSTALL \"${CMAKE_INSTALL_PREFIX}/bin/Contents/Plugins/\" DESTINATION \"${APP_INSTALL_PATH}/Contents/Plugins\")
            endif()

            # copy the other installed files or directories (not .app, lib, bin, share)
            file(GLOB FILES \"${CMAKE_INSTALL_PREFIX}/*\")

            foreach(FILE \${FILES})
                if (NOT \${FILE} MATCHES \"${EXECUTABLE_NAME}\"
                    AND NOT \${FILE} MATCHES \"${FWBUNDLE_RC_PREFIX}\"
                    AND NOT \${FILE} MATCHES \"${FWBUNDLE_LIB_PREFIX}\"
                    AND NOT \${FILE} MATCHES \"${CMAKE_INSTALL_LIBDIR}\"
                    AND NOT \${FILE} MATCHES \"bin\"
                )
                    message(\"install file  \${FILE}\")
                    file(INSTALL \${FILE} DESTINATION \"${APP_INSTALL_PATH}/Contents\")
                endif()
            endforeach()
        " COMPONENT ApplicationBundle)

        install(CODE "
            file(GLOB_RECURSE OGREPLUGINS \"${APP_INSTALL_PATH}/Contents/Plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
            file(GLOB_RECURSE QTPLUGINS \"${APP_INSTALL_PATH}/Contents/MacOS/qt5/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
            file(GLOB_RECURSE QMLPLUGINS \"${APP_INSTALL_PATH}/Contents/qml/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
            file(GLOB_RECURSE BUNDLES \"${APP_INSTALL_PATH}/Contents/lib/*/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

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
            fixup_bundle(\"${APP_INSTALL_PATH}\" \"\${BUNDLES_TO_FIX};\${QTPLUGINS};\${QMLPLUGINS};\${OGREPLUGINS}\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR};${FW_EXTERNAL_LIBRARIES_DIRS};\${BUNDLES_FOLDERS}\")

            execute_process(
                COMMAND sh -c \"find . -type f -name '*.dylib'|sed 's/\\\\.[0-9].*//g'|sort|uniq -d|xargs -I{} -n1 find . -path '{}.*' | paste -d ' ' - -| sed 's/^/ln -s -f /' | sh \"
                WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents/MacOS
                )

            execute_process(
                COMMAND sh -c \"python ${CMAKE_SOURCE_DIR}/CMake/install/macos/osx_install_name_tool.py -e MacOS/ $(find \"${FWBUNDLE_LIB_PREFIX}\" -iname '*.dylib') -f\"
                WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents
                )
        " COMPONENT ApplicationBundle)
    else()
        set(CPACK_OUTPUT_FILE_PREFIX packages)
        set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

        execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

        set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${VERSION}-macos_${ARCHITECTURE}-Sight_${GIT_TAG}")
        set(CPACK_PACKAGE_VENDOR "IRCAD-IHU")
        set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
        set(CPACK_PACKAGE_VERSION "${VERSION}")

        include(CPack)
    endif()

endfunction()
