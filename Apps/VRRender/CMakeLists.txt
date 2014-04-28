fwLoadProperties()


if(VRRender_INSTALL)
    if(APPLE)
        set(FWPROJECT_NAME VRRender)

        set(VRRENDER_BUNDLE_ICON_FILE app.icns) 
        set_source_files_properties(rc/${VRRENDER_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

        add_executable(
            VRRender.app MACOSX_BUNDLE
            ${launcher_HEADERS} ${launcher_SOURCES} 
            rc/${VRRENDER_BUNDLE_ICON_FILE}
            )

        set_target_properties( VRRender.app PROPERTIES OUTPUT_NAME ${FWPROJECT_NAME})

        add_definitions(-DDEFAULT_PROFILE=Bundles/${FWPROJECT_NAME}_${${FWPROJECT_NAME}_DASH_VERSION}/profile.xml)

        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_ICON_FILE ${VRRENDER_BUNDLE_ICON_FILE})
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_INFO_STRING "${FWPROJECT_NAME}")
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${${FWPROJECT_NAME}_VERSION}")
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${${FWPROJECT_NAME}_VERSION}")
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${FWPROJECT_NAME}")
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2014 IRCAD")
        set_target_properties( VRRender.app PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.ircad.vrrender")

        find_package (Boost COMPONENTS filesystem program_options REQUIRED)

        target_include_directories(VRRender.app PRIVATE ${Boost_INCLUDE_DIRS})
        target_link_libraries(VRRender.app fwCore fwRuntime ${Boost_LIBRARIES})

        set(APP_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/VRRender.app")

        configure_file("${fwGuiQt_DIR}/bin/qt.conf" "${APP_PATH}/Contents/Resources/qt.conf")
        INSTALL(CODE " file(WRITE \"${APP_PATH}/Contents/Resources/qt.conf\" \"\") " COMPONENT ApplicationBundle)

        install(
            TARGETS VRRender.app 
            BUNDLE DESTINATION . COMPONENT ApplicationBundle
            )

        set(APP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/VRRender.app")

        install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/share" DESTINATION "VRRender.app/Contents/" COMPONENT ApplicationBundle)
        install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/Bundles" DESTINATION "VRRender.app/Contents/" COMPONENT ApplicationBundle)
        install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/qtplugins" DESTINATION "VRRender.app/Contents/" COMPONENT ApplicationBundle)

        INSTALL(CODE "
        file(GLOB_RECURSE QTPLUGINS \"${APP_INSTALL_PATH}/Contents/qtplugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        file(GLOB_RECURSE BUNDLES \"${APP_INSTALL_PATH}/Contents/Bundles/*/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        include(BundleUtilities)
        fixup_bundle(\"${APP_INSTALL_PATH}\" \"\${BUNDLES};\${QTPLUGINS}\" \"${CMAKE_INSTALL_PREFIX}/${LIBRARY_OUTPUT_DIR}\")

        execute_process(
            COMMAND sh -c \"find . -type f -name '*.dylib'|sed 's/\\\\.[0-9].*//g'|sort|uniq -d|xargs -I{} -n1 find . -path '{}.*' | paste -d ' ' - -| sed 's/^/ln -s -f /' | sh \" 
            WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents/
            )
        execute_process(
            COMMAND sh -c \"python ${CMAKE_SOURCE_DIR}/CMake/OSXTools/osx_install_name_tool.py -e MacOS/ $(find Bundles -iname '*.dylib') -f\"
            WORKING_DIRECTORY ${APP_INSTALL_PATH}/Contents
            )

        " COMPONENT ApplicationBundle)
         
    elseif(WIN32)
        
        set(FWPROJECT_NAME VRRender)
        set(VRRENDER_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/rc/vrrender.ico) 
        set(LAUNCHER_PATH "bin/launcher.exe")
        set(PROFILE_PATH "Bundles/${FWPROJECT_NAME}_${${FWPROJECT_NAME}_DASH_VERSION}/profile.xml")
        
       #configure the wix template 
       configure_file(${FWCMAKE_RESOURCE_PATH}Wix/WIX.template.in ${PROJECT_BINARY_DIR}WIX.template [@ONLY] )
             
               
        install(CODE "
                      file(GLOB_RECURSE LIBS
                        \"\${CMAKE_INSTALL_PREFIX}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
                      include(BundleUtilities)
                      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/launcher.exe\" \"\${LIBS}\" \"${EXTERNAL_LIBRARIES}/bin\")
                     " COMPONENT Runtime)
                     
        set(CPACK_PACKAGE_NAME "${FWPROJECT_NAME}")
        set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging
        set(CPACK_PACKAGE_VENDOR "IRCAD")
        set(CPACK_PACKAGE_ICON ${VRRENDER_BUNDLE_ICON_FILE})
        set(CPACK_PACKAGE_VERSION "${VERSION}")
                 
        if(CPACK_BINARY_WIX)
       
            set(CPACK_WIX_TEMPLATE "${PROJECT_BINARY_DIR}WIX.template")
            set(CPACK_WIX_PROGRAM_MENU_FOLDER "IRCAD" )
            set(CPACK_WIX_PRODUCT_ICON ${VRRENDER_BUNDLE_ICON_FILE})
            set(CPACK_RESOURCE_FILE_LICENSE ${FWCMAKE_RESOURCE_PATH}Wix/LICENSE.rtf)
            
        endif(CPACK_BINARY_WIX)
        
        include(CPack)

    endif()
endif()



