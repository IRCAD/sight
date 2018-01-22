# CMake script file launch at build time before the build of each Bundle

# set variables used in the configure_file command
set(PROJECT_NAME ${PROJECT})

if(PROJECT_REQUIREMENTS)
    string(REPLACE " " ";" PROJECT_REQUIREMENTS ${PROJECT_REQUIREMENTS})

    # Add each requirements to the requirement list
    foreach(CURRENT_REQUIREMENT ${PROJECT_REQUIREMENTS})
        list(APPEND REQUIREMENT_LIST "    <requirement id=\"${CURRENT_REQUIREMENT}\"/>")
    endforeach()
endif()

# Add each bundle deps to the requirement list
foreach(CURRENT_BUNDLE_DEPS ${PROJECT_BUNDLE_DEPENDENCIES})
    list(APPEND REQUIREMENT_LIST "    <requirement id=\"${CURRENT_BUNDLE_DEPS}\"/>")
endforeach()

if(REQUIREMENT_LIST)
    list(SORT REQUIREMENT_LIST)
    string(REPLACE ";" "\n" REQUIREMENT_LIST "${REQUIREMENT_LIST}")
endif()

# Add each service to the extension list
file(TO_CMAKE_PATH "${PROJECT_DIR}/src" PRJ_SRC_DIR)
file(TO_CMAKE_PATH "${PROJECT_DIR}/include" PRJ_INCLUDE_DIR)

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
                                       "         <object>${SRV_OBJECT}</object>")
        else()
            list(APPEND EXTENSION_LIST "\n    <extension implements=\"::fwServices::registry::ServiceFactory\">"
                                       "         <type>${SRV_TYPE}</type>"
                                       "         <service>${SRV_IMPL}</service>")
        endif()

        set(SRV_REGISTER_OBJECT_REGEX "fwServicesRegisterObjectMacro\\(([ :a-zA-Z1-9_]+)[,\n\t\r ]*([ :a-zA-Z1-9_]*)\\);")

        file(STRINGS ${CPP_FILE} CPP_FILE_LINES_CONTENT)
        foreach(LINE ${CPP_FILE_LINES_CONTENT})
            if("${LINE}" MATCHES ${SRV_REGISTER_OBJECT_REGEX})
                if(CMAKE_MATCH_2)
                    string(STRIP ${CMAKE_MATCH_2} SRV_OBJECT)
                    list(APPEND EXTENSION_LIST "         <object>${SRV_OBJECT}</object>")
                endif()
            endif()
        endforeach()

        list(APPEND EXTENSION_LIST "         <desc>${SRV_DESC}</desc>"
                                   "    </extension>")
    else()

        # Guess everything from the doxygen

        # 1. Find the service type from fwCoreServiceClassDefinitionsMacro, we assume there is only one occurrence
        set(SRV_TYPE_REGEX "fwCoreServiceClassDefinitionsMacro\\([\n\t\r ]*\\([\n\t\r ]*([ :a-zA-Z1-9_]+)\\)[\n\t\r ]*\\([\n\t\r ]*([ :a-zA-Z1-9_]+)\\)")

        if("${HPP_FILE_CONTENT}" MATCHES ${SRV_TYPE_REGEX})

            string(STRIP ${CMAKE_MATCH_2} SRV_TYPE)


            # 2. Find the service implementation from the XML configuration doxygen
            set(SRV_IMPL_REGEX "<service[\n\t\r ]*(uid[\n\t\r ]*=[\n\t\r ]*\".*\")?[\n\t\r ]*type[\n\t\r ]*=[\n\t\r ]*\"([:a-zA-Z1-9_]*)\".*>")

            if("${HPP_FILE_CONTENT}" MATCHES ${SRV_IMPL_REGEX})

                string(STRIP ${CMAKE_MATCH_2} SRV_IMPL)
                file(STRINGS ${HPP_FILE} HPP_FILE_LINES_CONTENT)

                list(APPEND EXTENSION_LIST "    <extension implements=\"::fwServices::registry::ServiceFactory\">"
                                           "         <type>${SRV_TYPE}</type>"
                                           "         <service>${SRV_IMPL}</service>")
                list(APPEND REGISTER_SERVICES "fwServicesRegisterMacro( ${SRV_TYPE}, ${SRV_IMPL} )\n")

                # 3. Find the objects from input, inouts section
                set(OBJECT_REGEX "-[\n\t\r ]*\\\\b*[\n\t\r ]*([a-zA-Z1-9_]*)[\n\t\r ]*\\[(.*)\\]")
                foreach(LINE ${HPP_FILE_LINES_CONTENT})
                    if("${LINE}" MATCHES ${OBJECT_REGEX})
                        string(STRIP ${CMAKE_MATCH_1} OBJECT_KEY)
                        string(STRIP ${CMAKE_MATCH_2} OBJECT_IMPL)

                        list(APPEND EXTENSION_LIST "         <object key=\"${OBJECT_KEY}\">${OBJECT_IMPL}</object>")
                        list(APPEND REGISTER_SERVICES "fwServicesRegisterObjectMacro( ${SRV_IMPL}, ${OBJECT_IMPL} )\n")

                        set(OBJECT_INCLUDE_REGEX "::([a-zA-Z1-9_]*)::([a-zA-Z1-9_]*)")
                        if("${OBJECT_IMPL}" MATCHES ${OBJECT_INCLUDE_REGEX})
                            list(APPEND INCLUDE_SERVICES "#include <${CMAKE_MATCH_1}/${CMAKE_MATCH_2}.hpp>")
                        else()
                            message(FATAL_ERROR "Can't split namespace from object type: " ${OBJECT_IMPL})
                        endif()
                    endif()
                endforeach()

                list(APPEND EXTENSION_LIST "         <desc>${SRV_DESC}</desc>")

                # 4. Find the tags
                set(TAGS_REGEX "\\\\b*[\n\t\r ]*Tags[\n\t\r ]*:[\n\t\r]*([a-zA-Z1-9_ ,]*)")
                if("${HPP_FILE_CONTENT}" MATCHES ${TAGS_REGEX})
                    string(STRIP ${CMAKE_MATCH_1} SRV_TAGS)
                    list(APPEND EXTENSION_LIST "         <tags>${SRV_TAGS}</tags>")
                endif()

                list(APPEND EXTENSION_LIST "    </extension>\n")

                # 5. Add the include for each hpp file
                set(HPP_REGEX ".*/include/(.*)")
                if("${HPP_FILE}" MATCHES ${HPP_REGEX})
                    # Shorten include path
                    list(APPEND INCLUDE_SERVICES "#include \"${CMAKE_MATCH_1}\"")
                else()
                    # Full path otherwise
                    list(APPEND INCLUDE_SERVICES "#include \"${HPP_FILE}\"")
                endif()

                list(APPEND COPY_COMMAND_DEPENDS "${HPP_FILE}" )

            endif()

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

configure_file( "${CMAKE_SCRIPTS_DIR}/plugin.xml.in"
                "${PLUGIN_OUTPUT_PATH}/plugin.xml")

if(REGISTER_SERVICES)
    list(APPEND INCLUDE_SERVICES "#include <fwServices/macros.hpp>")

    string(REPLACE ";" "" REGISTER_SERVICES "${REGISTER_SERVICES}")
endif()

string(REPLACE ";" "\n" INCLUDE_SERVICES "${INCLUDE_SERVICES}")

configure_file( "${CMAKE_SCRIPTS_DIR}/registerServices.cpp.in"
                "${REGISTERSERVICES_OUTPUT_PATH}/registerServices.cpp")
