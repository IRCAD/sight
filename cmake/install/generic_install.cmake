if(WIN32)
    include(${FWCMAKE_INSTALL_FILES_DIR}/win_install.cmake)
elseif(UNIX)
    #Don't use rpath at install step
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    include(${FWCMAKE_INSTALL_FILES_DIR}/linux_install.cmake)
endif()

function(generic_install)

    get_property(SIGHT_APPS GLOBAL PROPERTY SIGHT_APPS)
    set_property(GLOBAL PROPERTY SIGHT_APPS ${SIGHT_APPS};${FWPROJECT_NAME} )

    if(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK)
        message(WARNING "generic_install() called twice for the target '${FWPROJECT_NAME}'.\nIt is probably present "
                        "in its CMakeLists.txt, please remove it.\ngeneric_install() will no longer be available in "
                        "version 22.0 of Sight.")
    else()
        set(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK 1)
        set(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK ${${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK} PARENT_SCOPE)

        set(CPACK_OUTPUT_FILE_PREFIX packages)
        set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging
        
        execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
        
        set(CPACK_GENERATOR TGZ)
        set(CPACK_SOURCE_GENERATOR TGZ)
        set(CPACK_PACKAGE_FILE_NAME "${FWPROJECT_NAME}-${SIGHT_VERSION}-linux_${ARCHITECTURE}-Sight_${GIT_TAG}")
        set(CPACK_PACKAGE_VENDOR "IRCAD")
        set(CPACK_PACKAGE_NAME "${FWPROJECT_NAME}")
        set(CPACK_PACKAGE_VERSION "${SIGHT_VERSION}")
        set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake")
        set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake")
        set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${FWPROJECT_NAME};ALL;.")
        include(CPack)

        if(WIN32)
            win_install(${FWPROJECT_NAME})
        elseif(UNIX)
            linux_install(${FWPROJECT_NAME})
        endif()
    endif()

endfunction()
