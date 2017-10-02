
function(plugin_setup ${PROJECT})

    #Clear last set
    unset(REQUIREMENT_LIST)
    unset(EXTENSION_LIST)
    unset(PROJECT_LIBRARY)
    unset(PROJECT_CLASS)

    # set variables used in the configure_file command
    set(PROJECT_VERSION ${${PROJECT}_VERSION})
    set(PROJECT_NAME ${PROJECT})
    set(PROJECT_ID ${PROJECT})

    # Add each requirements to the requirement list
    foreach(CURRENT_REQUIREMENT ${${PROJECT}_REQUIREMENTS})
        # to only consider bundles and app
        get_target_property(TARGET_TYPE ${CURRENT_REQUIREMENT} TYPE)
        if( "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "BUNDLE" OR "${${CURRENT_REQUIREMENT}_TYPE}" STREQUAL "APP")
                list(APPEND REQUIREMENT_LIST "    <requirement id=\"${CURRENT_REQUIREMENT}\"/>")
         endif()
    endforeach()

    # Add each bundle deps to the requirement list
    foreach(CURRENT_BUNDLE_DEPS ${${PROJECT}_BUNDLE_DEPENDENCIES})
        list(APPEND REQUIREMENT_LIST "    <requirement id=\"${CURRENT_BUNDLE_DEPS}\"/>")
    endforeach()

    if(REQUIREMENT_LIST)
        list(SORT REQUIREMENT_LIST)
        string(REPLACE ";" "\n" REQUIREMENT_LIST "${REQUIREMENT_LIST}")
    endif()

    # Add each service to the extension list
    file(TO_CMAKE_PATH "${${PROJECT}_DIR}/src" PRJ_SRC_DIR)
    file(TO_CMAKE_PATH "${${PROJECT}_DIR}/include" PRJ_INCLUDE_DIR)

    file(GLOB_RECURSE PRJ_CPP_FILES  "${PRJ_SRC_DIR}/*.cpp")
    foreach(CPP_FILE ${PRJ_CPP_FILES})
        #find associated .hpp file to find service description in doxygen
        file(TO_CMAKE_PATH ${CPP_FILE} CPP_FILE)
        string(REPLACE "${PRJ_SRC_DIR}" "" HPP_FILE ${CPP_FILE})
        string(REPLACE ".cpp" ".hpp" HPP_FILE ${HPP_FILE})
        set(HPP_FILE "${PRJ_INCLUDE_DIR}/${HPP_FILE}")
        file(TO_CMAKE_PATH ${HPP_FILE} HPP_FILE)

        unset(SRV_DESC)
        if(EXISTS ${HPP_FILE})
            set(BRIEF_REGEX "[ /*]*@brief ([^\n]+)")
            file(STRINGS ${HPP_FILE} HPP_FILE_CONTENT NEWLINE_CONSUME)
            if("${HPP_FILE_CONTENT}" MATCHES ${BRIEF_REGEX})
                string(STRIP ${CMAKE_MATCH_1} SRV_DESC)
            endif()
        endif()

        file(STRINGS ${CPP_FILE} CPP_FILE_CONTENT NEWLINE_CONSUME)
        set(SRV_REGISTER_REGEX "fwServicesRegisterMacro\\(([ :a-zA-Z1-9_]+)[,\n\t\r ]*([ :a-zA-Z1-9_]+)[,\n\t\r ]*([ :a-zA-Z1-9_]*)\\);")

        if("${CPP_FILE_CONTENT}" MATCHES ${SRV_REGISTER_REGEX})
            string(STRIP ${CMAKE_MATCH_1} SRV_TYPE)
            string(STRIP ${CMAKE_MATCH_2} SRV_IMPL)

            if(CMAKE_MATCH_3)
                string(STRIP ${CMAKE_MATCH_3} SRV_OBJECT)
                list(APPEND EXTENSION_LIST "\n    <extension implements=\"::fwServices::registry::ServiceFactory\">"
                                           "         <type>${SRV_TYPE}</type>"
                                           "         <service>${SRV_IMPL}</service>"
                                           "         <object>${SRV_OBJECT}</object>"
                                           "         <desc>${SRV_DESC}</desc>"
                                           "    </extension>")
            else()
                list(APPEND EXTENSION_LIST "\n    <extension implements=\"::fwServices::registry::ServiceFactory\">"
                                           "         <type>${SRV_TYPE}</type>"
                                           "         <service>${SRV_IMPL}</service>"
                                           "         <desc>${SRV_DESC}</desc>"
                                           "    </extension>")
            endif()
        endif()
    endforeach()
    if(EXTENSION_LIST)
        string(REPLACE ";" "\n" EXTENSION_LIST "${EXTENSION_LIST}")
    endif()

    # retrieves the class representing the bundle executable part.
    if(PRJ_CPP_FILES)
        set(PROJECT_CLASS "class=\"::${PROJECT_NAME}::Plugin\"")
        set(PROJECT_LIBRARY "    <library name=\"${PROJECT_NAME}\" />")
    endif()

    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/build/plugin.xml.in"
                    "${CMAKE_BINARY_DIR}/${BUNDLE_RC_PREFIX}/${PROJECT}-${${PROJECT}_VERSION}/plugin.xml")

endfunction()
