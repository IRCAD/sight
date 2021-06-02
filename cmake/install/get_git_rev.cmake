cmake_minimum_required(VERSION 3.0)

# CMake script to find:
# - last tag name

if(NOT GIT_TAG)
    # finds last tag name if you are exactly on (annotated) tag,
    # or <tag>-<n>-g<shortened sha-1> if not
    # where <n> is number of commits since <tag>
    execute_process(
        COMMAND git describe --tags
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# parse GIT_TAG information into pieces.
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" GIT_TAG_MAJOR "${GIT_TAG}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" GIT_TAG_MINOR "${GIT_TAG}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.(.*)" "\\1" GIT_TAG_PATCH "${GIT_TAG}")
