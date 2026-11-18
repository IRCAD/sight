# This script is used to download tests data on the continuous integration
# It takes two arguments to specify the output directory and the ref of the branch to checkout.
# Ex:
# cmake -DOUTPUT_FOLDER=/path/to -DBRANCH=dev -P download_test_data.cmake
#

# cmake-lint: disable=E1126

cmake_minimum_required(VERSION 3.19)

set(SIGHT_DATA_BASENAME "sight-data")
set(SIGHT_DATA_PUBLIC_URL "https://git.ircad.fr/sight/${SIGHT_DATA_BASENAME}")

if(NOT BRANCH)
    set(BRANCH "dev")
endif()
set(SIGHT_DATA_GET_REF_API "https://git.ircad.fr/api/v4/projects/700/repository/commits/${BRANCH}")

if(WIN32)
    set(CURL_CMD curl.exe)
else()
    set(CURL_CMD curl)
endif()

if(OUTPUT_FOLDER)
    set(OUTPUT_FOLDER "${OUTPUT_FOLDER}/")
endif()

# Get the exact sha1 corresponding to the branch we want to checkout
# This is necessary to differentiate checkout of the same branch at different commits
execute_process(COMMAND ${CURL_CMD} -k ${SIGHT_DATA_GET_REF_API} -s OUTPUT_VARIABLE COMMIT)
string(JSON COMMIT_SHORT_ID GET ${COMMIT} "short_id")
string(JSON COMMIT_ID GET ${COMMIT} "id")

set(SIGHT_DATA_REF ${OUTPUT_FOLDER}sight-data-${COMMIT_SHORT_ID})

# Use a lock file to prevent any other instance of this script to proceed while we work on that folder
file(LOCK ${SIGHT_DATA_REF}.lock)
set(SIGHT_DATA_FOLDER ${SIGHT_DATA_REF}-${COMMIT_ID})

if(EXISTS ${SIGHT_DATA_FOLDER})
    message(STATUS "Using existing test data for commit @" ${COMMIT_SHORT_ID})
else()
    message(STATUS "Download test data for commit @" ${COMMIT_SHORT_ID})

    set(SIGHT_DATA_ARCHIVE_API "https://git.ircad.fr/api/v4/projects/700/repository/archive.zip?sha=${COMMIT_SHORT_ID}")
    execute_process(
        COMMAND ${CURL_CMD} -k --fail -s -S ${SIGHT_DATA_ARCHIVE_API} -o ${SIGHT_DATA_REF}.zip
        RESULT_VARIABLE CURL_RESULT
    )
    if(NOT CURL_RESULT EQUAL 0)
        file(REMOVE "${SIGHT_DATA_REF}.zip")
        message(FATAL_ERROR "Download of the test data failed: ${CURL_RESULT}")
    endif()

    # Extract aside, then rename: an interrupted extraction must never be taken for the data of this commit
    set(STAGING_FOLDER ${SIGHT_DATA_FOLDER}.partial)
    file(REMOVE_RECURSE "${STAGING_FOLDER}")
    file(ARCHIVE_EXTRACT INPUT "${SIGHT_DATA_REF}.zip" DESTINATION "${STAGING_FOLDER}")
    file(REMOVE "${SIGHT_DATA_REF}.zip")
    file(GLOB EXTRACTED_FOLDER LIST_DIRECTORIES true "${STAGING_FOLDER}/*")
    list(LENGTH EXTRACTED_FOLDER EXTRACTED_COUNT)
    if(NOT EXTRACTED_COUNT EQUAL 1)
        message(FATAL_ERROR "The test data archive must hold one folder, not ${EXTRACTED_COUNT}")
    endif()
    file(RENAME "${EXTRACTED_FOLDER}" "${SIGHT_DATA_FOLDER}")
    file(REMOVE_RECURSE "${STAGING_FOLDER}")
endif()

message(STATUS "Test data available in")
# Output the path where we checkout the data, this will be captured by the calling CI script to set FWTEST_DATA_DIR
message("${SIGHT_DATA_REF}-${COMMIT_ID}")
