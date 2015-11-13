function(osx_install PRJ_NAME)

    set(EXECUTABLE_NAME ${PRJ_NAME}.app)
    
    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.icns)
    
    set_source_files_properties(rc/${ICON_FILENAME} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${launcher_HEADERS} ${launcher_SOURCES} rc/${ICON_FILENAME})
        
    add_definitions(-DDEFAULT_PROFILE=Bundles/${PRJ_NAME}_${${PRJ_NAME}_DASH_VERSION}/profile.xml)

    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME ${PRJ_NAME})
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_FILENAME})
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PRJ_NAME}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${${PRJ_NAME}_VERSION}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${${PRJ_NAME}_VERSION}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PRJ_NAME}")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2015 IRCAD")
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.ircad.${LOWER_PRJ_NAME}")

    find_package (Boost COMPONENTS program_options REQUIRED )
    
    target_link_libraries(${EXECUTABLE_NAME} 
        fwCore 
        fwRuntime 
        ${Boost_PROGRAM_OPTIONS_LIBRARIES} 
    )

    set(APP_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE_NAME}")
    set(APP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}")

    configure_file("${fwGuiQt_DIR}/bin/qt.conf" "${APP_PATH}/Contents/Resources/qt.conf")
    
    install(CODE " file(WRITE \"${APP_PATH}/Contents/Resources/qt.conf\" \"\") " COMPONENT ApplicationBundle)
    install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION . COMPONENT ApplicationBundle)
    install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/share" DESTINATION "${EXECUTABLE_NAME}/Contents/" COMPONENT ApplicationBundle)
    install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/Bundles" DESTINATION "${EXECUTABLE_NAME}/Contents/" COMPONENT ApplicationBundle)
    install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/qtplugins" DESTINATION "${EXECUTABLE_NAME}/Contents/" COMPONENT ApplicationBundle)


    install(CODE "
    file(GLOB_RECURSE QTPLUGINS \"${APP_INSTALL_PATH}/Contents/qtplugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    file(GLOB_RECURSE BUNDLES \"${APP_INSTALL_PATH}/Contents/Bundles/*/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APP_INSTALL_PATH}\" \"\${BUNDLES};\${QTPLUGINS}\" \"${CMAKE_INSTALL_PREFIX}/${LIBRARY_OUTPUT_DIR}\")


    execute_process(
        COMMAND sh -c \"find . -type f -name '*.dylib'|sed 's/\\\\.[0-9].*//g'|sort|uniq -d|xargs -I{} -n1 find . -path '{}.*' | paste -d ' ' - -| sed 's/^/ln -s -f /' | sh \"
        WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents/MacOS
        )


    execute_process(
        COMMAND sh -c \"python ${CMAKE_SOURCE_DIR}/CMake/OSXTools/osx_install_name_tool.py -e MacOS/ $(find Bundles -iname '*.dylib') -f\"
        WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents
        )


    " COMPONENT ApplicationBundle)
    
endfunction(osx_install)