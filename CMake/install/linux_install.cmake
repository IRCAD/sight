
#Find all sub-folders containing external libraries
function(findExtLibDir EXTERNAL_LIBRARIES_DIRECTORIES)
    file(GLOB_RECURSE LIBS ${EXTERNAL_LIBRARIES}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*)
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

    if(NOT USE_SYSTEM_LIB AND NOT BUILD_SDK)
        findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)
    endif()
    set(CPACK_GENERATOR TGZ)
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")

        set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
        set(LAUNCHER "fwlauncher-${fwlauncher_VERSION}")
        set(PROFILE_PATH "${PRJ_NAME}-${PROJECT_VERSION}/profile.xml")

    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")

        set(LAUNCHER_PATH "bin/${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(LAUNCHER "${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(PROFILE_PATH "")

    elseif(NOT BUILD_SDK)
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()

    if(NOT BUILD_SDK)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
        install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)
    endif()

    set(CPACK_OUTPUT_FILE_PREFIX packages)
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

    execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

    set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${VERSION}-linux_${ARCHITECTURE}")
    set(CPACK_PACKAGE_VENDOR "IRCAD-IHU")
    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_VERSION "${VERSION}")

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif()

    if(NOT USE_SYSTEM_LIB AND NOT BUILD_SDK)
        #Copy the qt font directory inside install/libs
        install(DIRECTORY "${EXTERNAL_LIBRARIES}/lib/fonts" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/")
    endif()

    include(CPack)

endmacro()
