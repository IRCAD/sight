
#Find all sub-folders containing external libraries
function(findExtLibDir EXTERNAL_LIBRARIES_DIRECTORIES)
    file(GLOB_RECURSE LIBS ${EXTERNAL_LIBRARIES}/*${CMAKE_SHARED_LIBRARY_SUFFIX}.*)
    list(REMOVE_DUPLICATES LIBS)
    set(FOLDERS)
    foreach(LIB ${LIBS})
        get_filename_component(LIB_DIR "${LIB}" DIRECTORY)
        list(APPEND FOLDERS ${LIB_DIR})
    endforeach()
    list(REMOVE_DUPLICATES FOLDERS)
    set(${EXTERNAL_LIBRARIES_DIRECTORIES} ${FOLDERS} PARENT_SCOPE)
endfunction()

#Linux install
macro(linux_install PRJ_NAME)
    findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)

    set(CPACK_GENERATOR TGZ)
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    set(LAUNCHER_PATH "bin/launcher-0.1")
    set(LAUNCHER "launcher-0.1")
    set(PROFILE_PATH "${PRJ_NAME}_${DASH_VERSION}/profile.xml")

    #configure the 'fixup' script
    configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)

    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

    set(CPACK_PACKAGE_VENDOR "IRCAD")
    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_VERSION "${VERSION}")

    configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}_${DASH_VERSION}.sh @ONLY)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PRJ_NAME}_${DASH_VERSION}.sh DESTINATION ${CMAKE_INSTALL_PREFIX} )
    #Copy the qt font directory inside install/libs
    install( DIRECTORY "${EXTERNAL_LIBRARIES}/lib/fonts" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/")

    include(CPack)

endmacro()
