# Function that returns the last git tag matching the prefix passed as parameter
# tag name if you are exactly on (annotated) tag,
# or <tag>-<n>-g<shortened sha-1> if not
# where <n> is number of commits since <tag>
function(get_last_git_tag PREFIX)

    if(PREFIX)
        execute_process(
            COMMAND git describe --tags --match "${PREFIX}*"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE LAST_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
        )

        # if the prefix does not succeed, try lower case
        string(TOLOWER ${PREFIX} LOWER_CASE_PREFIX)
        if(NOT LAST_TAG)
            execute_process(
                COMMAND git describe --tags --match "${LOWER_CASE_PREFIX}*"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                OUTPUT_VARIABLE LAST_TAG
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
            )
        endif()

        if(LAST_TAG)
            string(TOLOWER ${LAST_TAG} LOWER_CASE_LAST_TAG)
            string(REPLACE "${LOWER_CASE_PREFIX}/" "" LAST_TAG ${LOWER_CASE_LAST_TAG})
        endif()
    endif()

    # if the prefix does not succeed, default to the last tag matching a strictly numerical scheme
    if(NOT LAST_TAG)
        execute_process(
            COMMAND git describe --tags --match "[0-9]*"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE LAST_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
        )
    endif()

    # Well, we found nothing... In this case, we just use the last commit hash
    if(NOT LAST_TAG)
        execute_process(
            COMMAND git describe --always
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE LAST_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()

    set(GIT_TAG ${LAST_TAG} PARENT_SCOPE)
endfunction()
