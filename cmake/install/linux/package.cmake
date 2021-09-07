#Linux install
macro(linux_package PRJ_NAME)

    string(TOLOWER ${PRJ_NAME} LOWER_PRJ_NAME)
    set(ICON_FILENAME ${LOWER_PRJ_NAME}.ico)

    get_target_property(TARGET_TYPE ${SIGHT_TARGET} SIGHT_TARGET_TYPE)

    if("${TARGET_TYPE}" STREQUAL  "APP")

        set(LAUNCHER_PATH "bin/sightrun.bin")
        set(LAUNCHER "sightrun.bin")
        set(PROFILE_PATH "${PRJ_NAME}/profile.xml")

        if(${FW_BUILD_EXTERNAL})
            # install the launcher
            install(PROGRAMS "${Sight_BINARY_DIR}/${LAUNCHER}" DESTINATION "bin")
        endif()
    elseif("${TARGET_TYPE}" STREQUAL  "EXECUTABLE")

        set(LAUNCHER_PATH "bin/${PRJ_NAME}.bin")
        set(LAUNCHER "${PRJ_NAME}.bin")
        set(PROFILE_PATH "")

    elseif()
        message(FATAL_ERROR "'${PRJ_NAME}' is not an installable (type : ${TARGET_TYPE})")
    endif()

    if(${PRJ_NAME} STREQUAL "sight")
        # Needed for fixup_bundle first argument
        set(LAUNCHER_PATH "bin/sightrun.bin")
    endif()

    # TODO: Re-enable the fixup for sight-deps only
    #configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/linux_fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake @ONLY)
    #install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linux_fixup.cmake)

    if("${TARGET_TYPE}" STREQUAL  "APP")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} 
                DESTINATION ${CMAKE_INSTALL_PREFIX}/bin 
        )

    elseif("${TARGET_TYPE}" STREQUAL  "EXECUTABLE")
        string(TOLOWER ${PRJ_NAME} APP_NAME)
        configure_file(${FWCMAKE_RESOURCE_PATH}/install/linux/template_exe.sh.in ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME} @ONLY)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}
                DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        )
    endif()

    get_platform_package_suffix()

    set(CPACK_GENERATOR TZST)
    set(CPACK_SOURCE_GENERATOR TZST)
    set(CPACK_PACKAGE_NAME "${PRJ_NAME}")
    set(CPACK_PACKAGE_FILE_NAME "${PRJ_NAME}-${GIT_TAG}-${PLATFORM_SUFFIX}")
    set(CPACK_PACKAGE_VENDOR "IRCAD")
    set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${PRJ_NAME};ALL;.")
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_INSTALL_PREFIX};.")
    set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake")
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake")
    
    # CPackComponent doesn't work properly with the 'single project' packaging mechanism we hacked here
    # As a workaround, this script allows to remove the dev component files before packing
    set(CPACK_PRE_BUILD_SCRIPTS ${FWCMAKE_RESOURCE_PATH}/install/pre_package.cmake)

    include(CPack)
    
endmacro()
