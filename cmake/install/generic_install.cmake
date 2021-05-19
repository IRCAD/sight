if(WIN32)
    include(${FWCMAKE_INSTALL_FILES_DIR}/windows/package.cmake)
elseif(UNIX)
    #Don't use rpath at install step
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    include(${FWCMAKE_INSTALL_FILES_DIR}/linux_install.cmake)
endif()

function(generic_install)

    get_property(SIGHT_APPS GLOBAL PROPERTY SIGHT_APPS)
    set_property(GLOBAL PROPERTY SIGHT_APPS ${SIGHT_APPS};${FWPROJECT_NAME} )

    if(WIN32)
        win_package(${FWPROJECT_NAME})
    elseif(UNIX)
        linux_install(${FWPROJECT_NAME})
    endif()

endfunction()
