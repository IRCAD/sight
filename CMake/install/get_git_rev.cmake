cmake_minimum_required(VERSION 3.0)

# CMake script to find:
# - last tag name
# - name of the current branch
# - hash of the last commit on the current branch
# - date of the last commit on the current branch

# finds name of the current branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# finds hash of the last commit on the current branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# finds date of the last commit on the current branch
execute_process(
    COMMAND git log -1 --format=%ai
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# finds last tag name if you are exactly on (annotated) tag,
# or <tag>-<n>-g<shortened sha-1> if not
# where <n> is number of commits since <tag>
execute_process(
    COMMAND git describe --tags
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# parse GIT_TAG information into pieces.
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" GIT_TAG_MAJOR "${GIT_TAG}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" GIT_TAG_MINOR "${GIT_TAG}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.(.*)" "\\1" GIT_TAG_PATCH "${GIT_TAG}")
