include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/osx_install.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/install/win_install.cmake)

macro(generic_install)
    if(${FWPROJECT_NAME}_INSTALL)
        if(APPLE)
            osx_install(${FWPROJECT_NAME})
        elseif(WIN32)
            win_install(${FWPROJECT_NAME})
        endif()
    endif()
    
endmacro(generic_install)


