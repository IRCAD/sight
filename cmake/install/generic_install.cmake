if(WIN32)
    include(${FWCMAKE_INSTALL_FILES_DIR}/windows/package.cmake)
elseif(UNIX)
    #Don't use rpath at install step to be relocatable
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    include(${FWCMAKE_INSTALL_FILES_DIR}/linux/package.cmake)
endif()

function(generic_install)

    get_property(SIGHT_APPS GLOBAL PROPERTY SIGHT_APPS)
    set_property(GLOBAL PROPERTY SIGHT_APPS ${SIGHT_APPS};${SIGHT_TARGET} )

    if(WIN32)
        win_package(${SIGHT_TARGET})
    elseif(UNIX)
        linux_package(${SIGHT_TARGET})
    endif()

endfunction()

function(get_platform_package_suffix)

    if(UNIX)
        execute_process(COMMAND lsb_release -i -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_DISTRIBUTION)
        execute_process(COMMAND lsb_release -r -s COMMAND tr -d '\n' OUTPUT_VARIABLE LINUX_VERSION)
        execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

        set(PLATFORM_SUFFIX ${LINUX_DISTRIBUTION}-${LINUX_VERSION}-${CMAKE_BUILD_TYPE}-${ARCHITECTURE})
    else()
        set(PLATFORM_SUFFIX ${CMAKE_BUILD_TYPE}-win64)
    endif()
    set(PLATFORM_SUFFIX ${PLATFORM_SUFFIX} PARENT_SCOPE)
endfunction()