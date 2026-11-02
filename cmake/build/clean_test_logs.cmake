# Run by CTest before the tests (CTEST_CUSTOM_PRE_TEST): empties TEST_LOGS_DIR, unless SIGHT_TEST_LOG_KEEP is set.
# Never fails, or CTest would not run the tests.

if("$ENV{SIGHT_TEST_LOG_KEEP}")
    return()
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -rf "${TEST_LOGS_DIR}" RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET)

if(NOT result EQUAL 0 OR EXISTS "${TEST_LOGS_DIR}")
    message(WARNING "Could not empty '${TEST_LOGS_DIR}', previous test logs are kept")
endif()
