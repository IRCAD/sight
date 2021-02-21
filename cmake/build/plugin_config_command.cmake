# CMake script file launch at build time before the build of each module


file(STRINGS "${PROJECT_DIR}/rc/plugin.xml" PLUGIN_CONTENT)

list(APPEND REGISTER_INCLUDE "#include <service/macros.hpp>")

set(FOUND_EXTENSION OFF)
foreach(LINE ${PLUGIN_CONTENT})

    if(NOT FOUND_EXTENSION)
        # Look for service extension
        if("${LINE}" MATCHES "<extension implements=\"(::)?sight::service::extension::Factory\">")
            set(FOUND_EXTENSION ON)
            set(OBJECTS "")
        endif()
    else()
        if("${LINE}" MATCHES "</extension>")
            set(FOUND_EXTENSION OFF)

            # Generate an entry for one service
            list(APPEND REGISTER_SERVICES "fwServicesRegisterMacro( ${TYPE}, ${SERVICE} )\n")

            # Generate the include for the service
            set(SERVICE_INCLUDE ${SERVICE})
            string(REGEX REPLACE "^::(.*)" "\\1" SERVICE_INCLUDE ${SERVICE_INCLUDE})
            get_filename_component(PROJECT_LAST_DIR ${PROJECT_DIR} NAME)
            string(REGEX REPLACE ".*${PROJECT_LAST_DIR}::(.*)" "\\1" SERVICE_INCLUDE ${SERVICE_INCLUDE})
            string(REGEX REPLACE "::" "/" SERVICE_INCLUDE ${SERVICE_INCLUDE})
            list(APPEND REGISTER_INCLUDE "#include <${PROJECT_DIR}/${SERVICE_INCLUDE}.hpp>")

            foreach(OBJ ${OBJECTS})
                list(APPEND REGISTER_SERVICES "fwServicesRegisterObjectMacro( ${SERVICE}, ${OBJ} )\n")
                # Generate the include for the data objects

                set(OBJECT_INCLUDE ${OBJ})
                string(REGEX REPLACE "^::(.*)" "\\1" OBJECT_INCLUDE ${OBJECT_INCLUDE})
                string(REGEX REPLACE "[A-z0-9]*::(.*)" "\\1" OBJECT_INCLUDE ${OBJECT_INCLUDE})
                string(REGEX REPLACE "::" "/" OBJECT_INCLUDE ${OBJECT_INCLUDE})

                list(APPEND REGISTER_INCLUDE "#include <${OBJECT_INCLUDE}.hpp>")
            endforeach()

        else()
            if("${LINE}" MATCHES "<type>(.*)</type>")
                set(TYPE ${CMAKE_MATCH_1})
            elseif("${LINE}" MATCHES "<service>(.*)</service>")
                set(SERVICE ${CMAKE_MATCH_1})
            elseif("${LINE}" MATCHES "<object>(.*)</object>")
                list(APPEND OBJECTS ${CMAKE_MATCH_1})
            elseif("${LINE}" MATCHES "<object key=\"(.*)\">(.*)</object>")
                list(APPEND OBJECTS ${CMAKE_MATCH_2})
            endif()
        endif()
    endif()

endforeach()

if(REGISTER_SERVICES)
    list(APPEND REGISTER_INCLUDE "#include <service/macros.hpp>")

    string(REPLACE ";" "" REGISTER_SERVICES "${REGISTER_SERVICES}")
endif()

string(REPLACE ";" "\n" REGISTER_INCLUDE "${REGISTER_INCLUDE}")

configure_file( "${CMAKE_SCRIPTS_DIR}/registerServices.cpp.in"
                "${REGISTERSERVICE_OUTPUT_PATH}/registerServices.cpp")
