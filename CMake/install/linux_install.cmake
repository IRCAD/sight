
#Find all sub-folders containing external libraries
function(findExtLibDir EXTERNAL_LIBRARIES_DIRECTORIES)

    if(NOT EXTERNAL_LIBRARIES)
        message(FATAL_ERROR "EXTERNAL_LIBRARIES variable is missing. Please, specify external libraries location to generate CMake projects.")
    endif()

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

    if(NOT USE_SYSTEM_LIB AND NOT BUILD_SDK AND NOT USE_CONAN)
        findExtLibDir(EXTERNAL_LIBRARIES_DIRECTORIES)
    endif()
    set(CPACK_GENERATOR TGZ)
    set(CPACK_SOURCE_GENERATOR TGZ)
    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")

        set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
        set(LAUNCHER "fwlauncher-${fwlauncher_VERSION}")
        set(PROFILE_PATH "${PRJ_NAME}-${PROJECT_VERSION}/profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${${PRJ_NAME}_TYPE}" STREQUAL  "EXECUTABLE")

        set(LAUNCHER_PATH "bin/${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(LAUNCHER "${PRJ_NAME}-${${PRJ_NAME}_VERSION}")
        set(PROFILE_PATH "")

    elseif(NOT BUILD_SDK)
        message(FATAL_ERROR "'${PRJ_NAME}' is not a installable (type : ${${PRJ_NAME}_TYPE})")
    endif()

    if(NOT BUILD_SDK)
        set(PROJECT_REQUIREMENTS ${${PROJECT}_REQUIREMENTS})

        if(${FW_BUILD_EXTERNAL})
            # install requirements
            findAllDependencies("${PROJECT}" PROJECT_LIST)

            foreach(REQUIREMENT ${PROJECT_LIST})
                if(${REQUIREMENT}_EXTERNAL)
                    # search and setup qt plugins for each bundles
                    qt_plugins_setup(${REQUIREMENT})

                    if(EXISTS "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                        install(DIRECTORY "${Sight_LIBRARY_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${FWBUNDLE_LIB_PREFIX})
                    endif()
                    if(EXISTS "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}")
                        install(DIRECTORY "${Sight_BUNDLES_DIR}/${REQUIREMENT}-${${REQUIREMENT}_VERSION}" DESTINATION ${FWBUNDLE_RC_PREFIX})
                    endif()
                endif()
            endforeach()

            install_qt_plugins()
        endif()
    endif()

    if(NOT BUILD_SDK OR (BUILD_SDK AND ${PRJ_NAME} STREQUAL "sight") )
        if(${PRJ_NAME} STREQUAL "sight")
            # Needed for fixup_bundle first argument
            set(LAUNCHER_PATH "bin/fwlauncher-${fwlauncher_VERSION}")
        endif()

        if(NOT BUILD_SDK)
            configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
            install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)
        else()
            installConanDepsForSDK()
        endif()

        set(CPACK_OUTPUT_FILE_PREFIX packages)
        set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.") #look inside install dir for packaging

        execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

        set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${VERSION}-linux_${ARCHITECTURE}")
        set(CPACK_PACKAGE_VENDOR "IRCAD-IHU")
        set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
        set(CPACK_PACKAGE_VERSION "${VERSION}")
    endif()

    if("${${PRJ_NAME}_TYPE}" STREQUAL  "APP")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif()

    include(CPack)

endmacro()
