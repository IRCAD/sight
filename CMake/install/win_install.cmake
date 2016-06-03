
#Find all sub-folders containing external libraries
function(findExtLibDir EXTERNAL_LIBRARIES_DIRECTORIES)
    file(GLOB_RECURSE LIBS ${EXTERNAL_LIBRARIES}/*${CMAKE_SHARED_LIBRARY_SUFFIX})
    list(REMOVE_DUPLICATES LIBS)
    set(FOLDERS)
    foreach(LIB ${LIBS})
        get_filename_component(LIB_DIR "${LIB}" DIRECTORY)
        list(APPEND FOLDERS ${LIB_DIR})
    endforeach()
    list(REMOVE_DUPLICATES FOLDERS)
    set(${EXTERNAL_LIBRARIES_DIRECTORIES} ${FOLDERS} PARENT_SCOPE)
endfunction()

#Windows install
macro(win_install PRJ_NAME)
    findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)
    
    set(CPACK_GENERATOR NSIS)
    
    #set app icon filename
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)    
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)
    
    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        set(LAUNCHER_PATH "bin/fwlauncher.exe")
        set(PROFILE_PATH "${${PRJ_NAME}_BUNDLE_DIR}/profile.xml")
    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")
        set(LAUNCHER_PATH "bin/${PRJ_NAME}.exe")
        set(PROFILE_PATH "")
    else()
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()
    
    list(APPEND CMAKE_MODULE_PATH ${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/)

    #configure the 'fixup' script
    configure_file(${FWCMAKE_RESOURCE_PATH}/install/windows/windows_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/windows_fixup.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/windows_fixup.cmake)
    
    if(CMAKE_CL_64)
        set(CPACK_NSIS_INSTALL_DIR "$PROGRAMFILES64")
    endif()

    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

    set(CPACK_PACKAGE_VENDOR "FW4SPL")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/fw4spl-org")
    set(CPACK_NSIS_CONTACT "fw4spl@gmail.com")

    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME "${PRJ_NAME}_${${PRJ_NAME}_DASH_VERSION}")
    set(CPACK_NSIS_DISPLAY_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_VERSION "${VERSION}")
    
    set(DEFAULT_NSIS_RC_PATH "${FWCMAKE_RESOURCE_PATH}/install/windows/NSIS/rc/")
    
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
    
    string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_WELCOMEFINISH_IMAGE ${CPACK_NSIS_WELCOMEFINISH_IMAGE})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON ${CPACK_NSIS_MUI_ICON})
    string(REPLACE "/" "\\\\" CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_UNIICON})
    string(REPLACE "/" "\\\\" CPACK_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
    
    set(CPACK_NSIS_DESKTOP_ICON ${ICON_FILENAME})
    set(CPACK_NSIS_SHORTCUT_ICON ${ICON_FILENAME})
    
    include(CPack)
    
endmacro()
