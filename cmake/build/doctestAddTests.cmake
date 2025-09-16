# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

set(prefix "${TEST_PREFIX}")
set(suffix "${TEST_SUFFIX}")
set(spec ${TEST_SPEC})
set(extra_args ${TEST_EXTRA_ARGS})
set(properties ${TEST_PROPERTIES})
set(add_labels ${TEST_ADD_LABELS})
set(junit_output_dir "${TEST_JUNIT_OUTPUT_DIR}")
set(script)
set(suite)
set(tests)

# Add a command to the script of the target
function(add_command NAME)
    set(_args "")
    foreach(_arg ${ARGN})
        if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
            set(_args "${_args} [==[${_arg}]==]") # form a bracket_argument
        else()
            set(_args "${_args} ${_arg}")
        endif()
    endforeach()
    set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# Run test executable to get list of available tests
if(NOT EXISTS "${TEST_EXECUTABLE}")
    message(FATAL_ERROR "Specified test executable '${TEST_EXECUTABLE}' does not exist")
endif()

if("${spec}" MATCHES .)
    set(spec "--test-case=${spec}")
endif()

# Horrible build of environment variables, one for the discovery, one for the execution
foreach(PATH ${TEST_ENV})
    set(DISCOVERY_ENV --modify PATH=path_list_append:${PATH} ${DISCOVERY_ENV})
    set(EXECUTION_ENV ${EXECUTION_ENV} ENVIRONMENT_MODIFICATION PATH=path_list_append:${PATH} )
endforeach()

if(WIN32)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env ${DISCOVERY_ENV} "${TEST_EXECUTABLE}" ${spec} --list-test-cases -r=xml
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${TEST_WORKING_DIR}"
    )
else()
    execute_process(
        COMMAND "${TEST_EXECUTABLE}" ${spec} --list-test-cases -r=xml
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${TEST_WORKING_DIR}"
    )
endif()
if(NOT ${result} EQUAL 0)
    message(FATAL_ERROR "Error running test executable '${TEST_EXECUTABLE}':\n" "  Result: ${result}\n"
                        "  Output: ${output}\n"
    )
endif()

string(REPLACE "\n" ";" output "${output}")

# Parse output
foreach(line ${output})

    if(NOT "${line}" MATCHES
       "<TestCase name=\"([A-z0-9:]*)\" (testsuite=\"([A-z0-9:]*)\" )?filename=\"([/\\_A-z\.:]*)\" line=\"([0-9]*)\""
    )
        continue()
    endif()
    set(test ${CMAKE_MATCH_1})
    if(NOT ${CMAKE_MATCH_3} STREQUAL "")
        set(prefix "${CMAKE_MATCH_3}::")
    endif()
    set(FILENAME ${CMAKE_MATCH_4})
    set(LINE_NUMBER ${CMAKE_MATCH_5})

    # use escape commas to handle properly test cases with commas inside the name
    string(REPLACE "," "\\," test_name ${test})
    # ...and add to script
    add_command(
        add_test "${prefix}${test}${suffix}" "${TEST_EXECUTABLE}" "--test-case=${test_name}"
        ${extra_args}
    )
    if(WIN32)
        add_command(
            set_tests_properties
            "${prefix}${test}${suffix}"
            PROPERTIES
            WORKING_DIRECTORY
            "${TEST_WORKING_DIR}"
            DEF_SOURCE_LINE
            "${FILENAME}:${LINE_NUMBER}"
            "${EXECUTION_ENV}"
        )
    else()
        add_command(
            set_tests_properties
            "${prefix}${test}${suffix}"
            PROPERTIES
            WORKING_DIRECTORY
            "${TEST_WORKING_DIR}"
            DEF_SOURCE_LINE
            "${FILENAME}:${LINE_NUMBER}"
        )
    endif()
    unset(labels)
    unset(prefix)
    list(APPEND tests "${prefix}${test}${suffix}")
endforeach()

# Create a list of all discovered tests, which users may use to e.g. set
# properties on the tests
add_command(set ${TEST_LIST} ${tests})

# Write CTest script
file(WRITE "${CTEST_FILE}" "${script}")
