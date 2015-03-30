fwLoadProperties()

if(${FWPROJECT_NAME}_INSTALL)
    if(APPLE)
        
        osx_install(${FWPROJECT_NAME})
        
    elseif(WIN32)
        
        set(BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/rc/arcalibration.ico) 
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
        set(CPACK_PACKAGE_ICON ${BUNDLE_ICON_FILE})
        set(CPACK_PACKAGE_VERSION "${VERSION}")
                 
        if(CPACK_BINARY_WIX)
       
            set(CPACK_WIX_TEMPLATE "${PROJECT_BINARY_DIR}WIX.template")
            set(CPACK_WIX_PROGRAM_MENU_FOLDER "IRCAD" )
            set(CPACK_WIX_PRODUCT_ICON ${BUNDLE_ICON_FILE})
            set(CPACK_RESOURCE_FILE_LICENSE ${FWCMAKE_RESOURCE_PATH}Wix/LICENSE.rtf)
            
        endif(CPACK_BINARY_WIX)
        
        include(CPack)
        
    endif()
endif()
