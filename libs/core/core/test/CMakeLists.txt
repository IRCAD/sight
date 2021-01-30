fwLoadProperties()

if(UNIX)
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DCAMP_LIB_DIR="/lib/x86_64-linux-gnu")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
    target_link_libraries(${FWPROJECT_NAME} PUBLIC "dl")
endif()
