if(APPLE)
    include(${FWCMAKE_INSTALL_FILES_DIR}/osx_install.cmake)
elseif(WIN32)
    include(${FWCMAKE_INSTALL_FILES_DIR}/win_install.cmake)
elseif(UNIX)
    #Don't use rpath at install step
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    include(${FWCMAKE_INSTALL_FILES_DIR}/linux_install.cmake)
endif()

macro(generic_install)
    if(${FWPROJECT_NAME}_INSTALL AND NOT BUILD_SDK)

        #qt plugins setup
        if(QT_REQUIREMENTS) # set by helper.cmake -> qt_setup() macros
             foreach(QT_REQUIREMENT ${QT_REQUIREMENTS})
                 get_filename_component(QT_REQ_DIR ${QT_REQUIREMENT} DIRECTORY)
                 install(DIRECTORY "${EXTERNAL_LIBRARIES}/${QT_REQUIREMENT}" DESTINATION "${QT_REQ_DIR}" )
            endforeach()

        endif()
        if(APPLE)
            osx_install(${FWPROJECT_NAME})
        elseif(WIN32)
            win_install(${FWPROJECT_NAME})
        elseif(UNIX)
            linux_install(${FWPROJECT_NAME})
        endif()
    endif()

endmacro(generic_install)
