
# Do not change the indentation of the activate list
macro(profile_setup ${PROJECT})
    set(UNIQUE "false")
    #Set the check-single-instance
    if(${PROJECT}_UNIQUE)
        string(TOLOWER "${${PROJECT}_UNIQUE}" UNIQUE)
    endif()

    #Clear last set
    unset(XML_ACTIVATE)
    # unset start for the current app
    unset(START_MODULES)
    unset(XML_START_MODULES)

    # set a variable used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})

    # Add each requirements to the activate list
    set(ALL_REQUIREMENTS "")
    findRequirements(${PROJECT} ALL_REQUIREMENTS)
    list(REMOVE_DUPLICATES ALL_REQUIREMENTS)
    list(SORT ALL_REQUIREMENTS)

    # Manage module starting
    if(${PROJECT}_START)
        list(APPEND START_MODULES "${PROJECT}")
    endif()

    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})
        # get the start option of the current requirement if exists
        if(${CURRENT_REQUIREMENT}_START)
            list(APPEND START_MODULES "${CURRENT_REQUIREMENT}")
        endif()
    endforeach()

    list(SORT START_MODULES)

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

        # to only consider modules and app
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "MODULE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            # check if a moduleParam macro had been use in the properties.cmake
            # if yes, get and set module param and values
            if(${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST)
                set(CURRENT_PARAM_LIST "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST}")
                set(CURRENT_PARAM_VALUES "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_VALUES}")

                #set activate tag with parameters
                list(APPEND XML_ACTIVATE "    <activate id=\"sight_${CURRENT_REQUIREMENT}\" version=\"${${CURRENT_REQUIREMENT}_VERSION}\" >")
                foreach(CURRENT_PARAM ${CURRENT_PARAM_LIST})
                    list(FIND CURRENT_PARAM_LIST "${CURRENT_PARAM}" CURRENT_INDEX)
                    list(GET CURRENT_PARAM_VALUES "${CURRENT_INDEX}" CURRENT_VALUE)
                    list(APPEND XML_ACTIVATE "        <param id=\"${CURRENT_PARAM}\" value=\"${CURRENT_VALUE}\" />")
                endforeach()

                list(APPEND XML_ACTIVATE "    </activate>")
            # else simply set the activate tag
            else()
                 list(APPEND XML_ACTIVATE "    <activate id=\"sight_${CURRENT_REQUIREMENT}\" version=\"${${CURRENT_REQUIREMENT}_VERSION}\" />")
            endif()
        endif()
    endforeach()
    string(REPLACE ";" "\n" XML_ACTIVATE "${XML_ACTIVATE}")

    foreach(CURRENT_MODULE ${START_MODULES})
        set(XML_START_MODULES "${XML_START_MODULES}\n    <start id=\"sight_${CURRENT_MODULE}\" />")
    endforeach()

    configure_file( "${FWCMAKE_BUILD_FILES_DIR}/profile.xml.in"
                    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${PROJECT}-${${PROJECT}_VERSION}/profile.xml")
endmacro()

function(findRequirements FWPROJECT_NAME)
    list(APPEND ALL_REQUIREMENTS ${FWPROJECT_NAME})

    set(CURRENT_REQUIREMENTS ${${FWPROJECT_NAME}_REQUIREMENTS})
    if(${FWPROJECT_NAME}_MODULE_DEPENDENCIES)
        list(APPEND CURRENT_REQUIREMENTS ${${FWPROJECT_NAME}_MODULE_DEPENDENCIES})
        list(REMOVE_DUPLICATES CURRENT_REQUIREMENTS)
    endif()

    foreach(CURRENT_REQUIREMENT ${CURRENT_REQUIREMENTS})
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "MODULE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            findRequirements(${CURRENT_REQUIREMENT})
        endif()
    endforeach()

    set(ALL_REQUIREMENTS ${ALL_REQUIREMENTS} PARENT_SCOPE)
endfunction()

macro(bundleParam MODULE_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PARAM_VALUES PARAM_LIST)
    cmake_parse_arguments("${NAME}_${MODULE_NAME}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
endmacro()

macro(moduleParam MODULE_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PARAM_VALUES PARAM_LIST)
    cmake_parse_arguments("${NAME}_${MODULE_NAME}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
endmacro()
