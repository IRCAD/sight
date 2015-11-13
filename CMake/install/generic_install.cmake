include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/osx_install.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/win_install.cmake)

macro(generic_install)
    if(${FWPROJECT_NAME}_INSTALL)
        
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
        endif()
    endif()
    
endmacro(generic_install)


