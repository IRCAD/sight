fwLoadProperties()

# Treat warnings as errors
if ( CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${FWPROJECT_NAME} PRIVATE "-Werror")
endif ()
