# Do not change the indentation of the activate list
macro(profile_setup PROJECT REQUIREMENTS)
    set(UNIQUE "false")
    set(NAME "${PROJECT}")

    #Set the check-single-instance
    get_target_property(UNIQUE ${PROJECT} SIGHT_UNIQUE)

    # set a variable used in the configure_file command
    get_target_property(PROJECT_VERSION ${PROJECT} SIGHT_PROJECT_VERSION)

    set(START_MODULES "")
    set(PRIORITY_MODULES "")
    set(ALL_REQUIREMENTS ${REQUIREMENTS})

    # Manage module starting
    get_target_property(START ${PROJECT} SIGHT_START)
    if(START)
        list(APPEND START_MODULES "${PROJECT}")
        list(APPEND PRIORITY_MODULES "100")
    endif()

    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})
        # get the start option of the current requirement if exists
        get_target_property(START ${CURRENT_REQUIREMENT} SIGHT_START)
        if(START)
            list(APPEND START_MODULES "${CURRENT_REQUIREMENT}")
        endif()
    endforeach()

    list(APPEND ALL_REQUIREMENTS ${PROJECT})

    set(XML_ACTIVATE "")
    set(XML_START_MODULES "")

    # Manage module activation
    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})

        get_target_property(TYPE ${CURRENT_REQUIREMENT} SIGHT_TARGET_TYPE)

        if("${TYPE}" STREQUAL "MODULE" OR "${TYPE}" STREQUAL "APP")

            get_target_property(MODULE_ID ${CURRENT_REQUIREMENT} SIGHT_MODULE_ID)

            get_target_property(PARAM_MODULE ${PROJECT} PARAM_MODULE)

            # check if a module_param macro had been used in the CMakeLists.txt
            # if yes, get and set module param and values
            if(${CURRENT_REQUIREMENT} IN_LIST PARAM_MODULE)
                # Tiny hack because ':' is not supported in variables names...
                string(REPLACE ":" "|" CURRENT_REQUIREMENT_REV "${CURRENT_REQUIREMENT}")
                get_target_property(CURRENT_PARAM_LIST ${PROJECT} ${CURRENT_REQUIREMENT_REV}_PARAM_LIST)
                get_target_property(CURRENT_PARAM_VALUES ${PROJECT} ${CURRENT_REQUIREMENT_REV}_PARAM_VALUES)

                #set activate tag with parameters
                list(APPEND XML_ACTIVATE "    <activate id=\"${MODULE_ID}\" >")
                foreach(CURRENT_PARAM ${CURRENT_PARAM_LIST})
                    list(FIND CURRENT_PARAM_LIST "${CURRENT_PARAM}" CURRENT_INDEX)
                    list(GET CURRENT_PARAM_VALUES "${CURRENT_INDEX}" CURRENT_VALUE)
                    list(APPEND XML_ACTIVATE "        <param id=\"${CURRENT_PARAM}\" value=\"${CURRENT_VALUE}\" />")
                endforeach()

                list(APPEND XML_ACTIVATE "    </activate>")
            else()
                list(APPEND XML_ACTIVATE "    <activate id=\"${MODULE_ID}\" />")
            endif()
        endif()
    endforeach()

    string(REPLACE ";" "\n" XML_ACTIVATE "${XML_ACTIVATE}")

    foreach(CURRENT_MODULE ${START_MODULES})
        get_target_property(MODULE_ID ${CURRENT_MODULE} SIGHT_MODULE_ID)

        set(XML_START_MODULES "${XML_START_MODULES}\n    <start id=\"${MODULE_ID}\" />")
    endforeach()

    configure_file(
        "${FWCMAKE_BUILD_FILES_DIR}/profile.xml.in"
        "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${PROJECT}/profile.xml"
    )
endmacro()

# Parse module parameters
macro(module_param MODULE_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PARAM_VALUES PARAM_LIST)

    # Tiny hack because ':' is not supported in variables names...
    string(REPLACE ":" "|" MODULE_NAME_REV "${MODULE_NAME}")
    cmake_parse_arguments(
        "${SIGHT_TARGET}_${MODULE_NAME_REV}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    get_target_property(PARAM_LIST ${SIGHT_TARGET} PARAM_MODULES)
    list(APPEND PARAM_MODULES ${MODULE_NAME})

    set_target_properties(
        ${SIGHT_TARGET}
        PROPERTIES PARAM_MODULE "${PARAM_MODULES}" ${MODULE_NAME_REV}_PARAM_LIST
                                                   "${${SIGHT_TARGET}_${MODULE_NAME_REV}_PARAM_LIST}"
                   ${MODULE_NAME_REV}_PARAM_VALUES "${${SIGHT_TARGET}_${MODULE_NAME_REV}_PARAM_VALUES}"
    )
endmacro()
