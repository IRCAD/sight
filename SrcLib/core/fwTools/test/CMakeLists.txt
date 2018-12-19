fwLoadProperties()

find_package(CppUnit QUIET REQUIRED)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

if(WIN32)
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE -DCAMP_LIB_DIR="${CONAN_BIN_DIRS_CAMP}")
else()
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE
                                CAMP_LIB_DIR="${CONAN_LIB_DIRS_CAMP}"
                                BUNDLE_LIB_PREFIX="${FWBUNDLE_LIB_PREFIX}")

    target_link_libraries(${FWPROJECT_NAME} PUBLIC "dl")
endif()
