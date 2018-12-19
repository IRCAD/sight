fwLoadProperties()

if(WIN32)
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DCAMP_LIB_DIR="${CONAN_BIN_DIRS_CAMP}")
else()
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DCAMP_LIB_DIR="${CONAN_LIB_DIRS_CAMP}")
endif()

