
fwLoadProperties()

# In the following we link fwRuntime_obj and not fwRuntime because we enabled GENERATE_OBJECT_LIB

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${LIBXML2_INCLUDE_DIR})
target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${LIBXML2_LIBRARIES})

if(WIN32)
    target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${Iconv_INCLUDE_DIR})
endif()
