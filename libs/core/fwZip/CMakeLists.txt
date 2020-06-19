fwLoadProperties()

find_package(ZLIB QUIET REQUIRED )
fwInclude(${ZLIB_INCLUDE_DIRS}) # No forward, minizip should be private
fwLink(${ZLIB_LIBRARIES})

target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DINCLUDECRYPTINGCODE_IFCRYPTALLOWED")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DHAVE_AES")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DUSE_INTEL_AES_IF_PRESENT")
