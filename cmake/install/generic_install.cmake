if(WIN32)
    include(${FWCMAKE_INSTALL_FILES_DIR}/win_install.cmake)
elseif(UNIX)
    #Don't use rpath at install step
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    include(${FWCMAKE_INSTALL_FILES_DIR}/linux_install.cmake)
endif()

function(generic_install)

    if(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK)
        message(WARNING "generic_install() called twice for the target '${FWPROJECT_NAME}'.\nIt is probably present "
                        "in its CMakeLists.txt, please remove it.\ngeneric_install() will no longer be available in "
                        "version 22.0 of Sight.")
    else()
        set(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK 1)
        set(${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK ${${FWPROJECT_NAME}_GENERIC_INSTALL_API_CHECK} PARENT_SCOPE)

        if(VCPKG_TARGET_TRIPLET)
            set(CPACK_PACKAGE_VERSION_MAJOR ${GIT_TAG_MAJOR})
            set(CPACK_PACKAGE_VERSION_MINOR ${GIT_TAG_MINOR})
            set(CPACK_PACKAGE_VERSION_PATCH ${GIT_TAG_PATCH})
            set(CPACK_PACKAGE_NAME ${FWPROJECT_NAME}-${CMAKE_BUILD_TYPE})
            set(CPACK_PACKAGE_VENDOR "IRCAD")
            set(CPACK_GENERATOR TGZ)
            include(CPack)
        else()
            if(WIN32)
                win_install(${FWPROJECT_NAME})
            elseif(UNIX)
                linux_install(${FWPROJECT_NAME})
            endif()
        endif()
    endif()

endfunction()
