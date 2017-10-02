
# Do not change the indentation of the activate list
macro(profile_setup ${PROJECT})
    set(UNIQUE "false")
    #Set the check-single-instance
    if(${PROJECT}_UNIQUE)
        string(TOLOWER "${${PROJECT}_UNIQUE}" UNIQUE)
    endif()

    #Clear last set
    unset(ACTIVATE_LIST)
    # unset start for the current app
    unset(START_BUNDLES)
    unset(START_BUNDLE_LIST)

    # set a variable used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})

    # Add each requirements to the activate list
    set(ALL_REQUIREMENTS "")
    findRequirements(${PROJECT} ALL_REQUIREMENTS)
    list(REMOVE_DUPLICATES ALL_REQUIREMENTS)
    list(SORT ALL_REQUIREMENTS)

    # Manage bundle starting
    # add a start for ctrl app
    if(${PROJECT}_START)
        list(APPEND START_BUNDLES "${PROJECT}")
    endif()

    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})
        # get the start option of the current requirement if exists
        if(${CURRENT_REQUIREMENT}_START)
            list(APPEND START_BUNDLES "${CURRENT_REQUIREMENT}")
        endif()
    endforeach()

    list(SORT START_BUNDLES)

    # Manage bundle activation
    foreach(CURRENT_REQUIREMENT ${ALL_REQUIREMENTS})

        # Ensure that we start this bundle before the "START_BEFORE"
        foreach(CURRENT_START_BEFORE ${${CURRENT_REQUIREMENT}_START_BEFORE})
            list(FIND START_BUNDLES ${CURRENT_START_BEFORE} INDEX_START_BEFORE )
            if(NOT ${INDEX_START_BEFORE} EQUAL -1)
                 list(INSERT START_BUNDLES ${INDEX_START_BEFORE} "${CURRENT_REQUIREMENT}")
                 break()
            endif()
        endforeach()

        # to only consider bundles and app
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "BUNDLE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            # check if a bundleParam macro had been use in the properties.cmake
            # if yes, get and set bundle param and values
            if(${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST)
                set(CURRENT_PARAM_LIST "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_LIST}")
                set(CURRENT_PARAM_VALUES "${${PROJECT}_${CURRENT_REQUIREMENT}_PARAM_VALUES}")

                #set activate tag with parameters
                list(APPEND ACTIVATE_LIST "    <activate id=\"${CURRENT_REQUIREMENT}\" version=\"${${CURRENT_REQUIREMENT}_VERSION}\" >")
                foreach(CURRENT_PARAM ${CURRENT_PARAM_LIST})
                    list(FIND CURRENT_PARAM_LIST "${CURRENT_PARAM}" CURRENT_INDEX)
                    list(GET CURRENT_PARAM_VALUES "${CURRENT_INDEX}" CURRENT_VALUE)
                    list(APPEND ACTIVATE_LIST "        <param id=\"${CURRENT_PARAM}\" value=\"${CURRENT_VALUE}\" />")
                endforeach()

                list(APPEND ACTIVATE_LIST "    </activate>")
            # else simply set the activate tag
            else()
                 list(APPEND ACTIVATE_LIST "    <activate id=\"${CURRENT_REQUIREMENT}\" version=\"${${CURRENT_REQUIREMENT}_VERSION}\" />")
            endif()
        endif()
    endforeach()
    string(REPLACE ";" "\n" ACTIVATE_LIST "${ACTIVATE_LIST}")

    foreach(CURRENT_BUNDLES ${START_BUNDLES})
        set(START_BUNDLE_LIST "${START_BUNDLE_LIST}\n    <start id=\"${CURRENT_BUNDLES}\" />")
    endforeach()

    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/profile.xml.in"
                    "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${PROJECT}-${${PROJECT}_VERSION}/profile.xml")
endmacro()

function(findRequirements FWPROJECT_NAME)
    list(APPEND ALL_REQUIREMENTS ${FWPROJECT_NAME})

    set(CURRENT_REQUIREMENTS ${${FWPROJECT_NAME}_REQUIREMENTS})
    if(${FWPROJECT_NAME}_BUNDLE_DEPENDENCIES)
        list(APPEND CURRENT_REQUIREMENTS ${${FWPROJECT_NAME}_BUNDLE_DEPENDENCIES})
        list(REMOVE_DUPLICATES CURRENT_REQUIREMENTS)
    endif()

    foreach(CURRENT_REQUIREMENT ${CURRENT_REQUIREMENTS})
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "BUNDLE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
            findRequirements(${CURRENT_REQUIREMENT})
        endif()
    endforeach()

    set(ALL_REQUIREMENTS ${ALL_REQUIREMENTS} PARENT_SCOPE)
endfunction()

macro(bundleParam BUNDLE_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PARAM_VALUES PARAM_LIST)
    cmake_parse_arguments("${NAME}_${BUNDLE_NAME}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
endmacro()
