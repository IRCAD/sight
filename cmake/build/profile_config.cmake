
# Do not change the indentation of the activate list
macro(profile_setup PROJECT)
    set(UNIQUE "false")

    #Set the check-single-instance
    get_target_property(UNIQUE ${PROJECT} SIGHT_UNIQUE)
    string(TOLOWER "${UNIQUE}" UNIQUE)

    # set a variable used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})

    get_target_property(ALL_REQUIREMENTS ${PROJECT} MANUALLY_ADDED_DEPENDENCIES)

    # Manage module starting
    get_target_property(START ${PROJECT} SIGHT_START)
    if(START)
        list(APPEND START_MODULES "${PROJECT}")
    endif()

    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})
        # get the start option of the current requirement if exists
        get_target_property(START ${CURRENT_REQUIREMENT} SIGHT_START)
        if(START)
            list(APPEND START_MODULES "${CURRENT_REQUIREMENT}")
        endif()
    endforeach()

    list(SORT START_MODULES)

    list(APPEND ALL_REQUIREMENTS "${PROJECT}")

    get_property(SIGHT_COMPONENTS GLOBAL PROPERTY SIGHT_COMPONENTS)

    # Manage module activation
    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})

        # Ensure that we start this module before the "START_BEFORE"
        foreach(CURRENT_START_BEFORE ${${CURRENT_REQUIREMENT}_START_BEFORE})
            list(FIND START_MODULES ${CURRENT_START_BEFORE} INDEX_START_BEFORE )
            if(NOT ${INDEX_START_BEFORE} EQUAL -1)
                 list(INSERT START_MODULES ${INDEX_START_BEFORE} "${CURRENT_REQUIREMENT}")
                 break()
            endif()
        endforeach()

        get_target_property(TYPE ${CURRENT_REQUIREMENT} SIGHT_TARGET_TYPE)
        
        if( "${TYPE}" STREQUAL "MODULE" OR "${TYPE}" STREQUAL "APP")
            
            string(REPLACE "_" "::" REQ ${CURRENT_REQUIREMENT})
            if(${CURRENT_REQUIREMENT} IN_LIST SIGHT_COMPONENTS)
                set(REQ "${PROJECT_NAME}::${REQ}")
            endif()

            # check if a moduleParam macro had been used in the CMakeLists.txt
            # if yes, get and set module param and values
            if(${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST)
                set(CURRENT_PARAM_LIST "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST}")
                set(CURRENT_PARAM_VALUES "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_VALUES}")

                #set activate tag with parameters
                list(APPEND XML_ACTIVATE "    <activate id=\"${REQ}\" >")
                foreach(CURRENT_PARAM ${CURRENT_PARAM_LIST})
                    list(FIND CURRENT_PARAM_LIST "${CURRENT_PARAM}" CURRENT_INDEX)
                    list(GET CURRENT_PARAM_VALUES "${CURRENT_INDEX}" CURRENT_VALUE)
                    list(APPEND XML_ACTIVATE "        <param id=\"${CURRENT_PARAM}\" value=\"${CURRENT_VALUE}\" />")
                endforeach()

                list(APPEND XML_ACTIVATE "    </activate>")
            endif()
        endif()
    endforeach()
    string(REPLACE ";" "\n" XML_ACTIVATE "${XML_ACTIVATE}")

    foreach(CURRENT_MODULE ${START_MODULES})
        string(REPLACE "_" "::" MODULE ${CURRENT_MODULE})
        if(${CURRENT_MODULE} IN_LIST SIGHT_COMPONENTS)
            set(MODULE "${PROJECT_NAME}::${MODULE}")
        endif()

        set(XML_START_MODULES "${XML_START_MODULES}\n    <start id=\"${MODULE}\" />")
    endforeach()

    configure_file( "${FWCMAKE_BUILD_FILES_DIR}/profile.xml.in"
                    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${PROJECT}/profile.xml")
endmacro()

macro(moduleParam MODULE_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PARAM_VALUES PARAM_LIST)
    cmake_parse_arguments("${FWPROJECT_NAME}_${MODULE_NAME}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
endmacro()
