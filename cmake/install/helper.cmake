# This script is called at install time only on linux OS
# It will be configured at configure time, only '@' variables will be replaced.
# function to find libs and modules directories

function(findLibsAndModules LIBS_MODULES_DIRECTORIES)
    file(GLOB_RECURSE LIBS ${CMAKE_INSTALL_PREFIX}/${SIGHT_MODULE_LIB_PREFIX}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*)
    if(${FW_BUILD_EXTERNAL})
        file(GLOB_RECURSE LIBS ${Sight_LIBRARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*)
    endif()

    list(REMOVE_DUPLICATES LIBS)
    set(FOLDERS)
    foreach(LIB ${LIBS})
        get_filename_component(LIB_DIR "${LIB}" DIRECTORY)
        list(APPEND FOLDERS ${LIB_DIR})
    endforeach()
    if(FOLDERS)
        list(REMOVE_DUPLICATES FOLDERS)
    endif()
    set(${LIBS_MODULES_DIRECTORIES} ${FOLDERS} PARENT_SCOPE)
endfunction()
