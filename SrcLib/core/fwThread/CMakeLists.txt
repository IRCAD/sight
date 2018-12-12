fwLoadProperties()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DWIN32_LEAN_AND_MEAN")
endif()
