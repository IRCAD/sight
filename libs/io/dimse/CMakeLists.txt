sight_add_target(io_dimse TYPE LIBRARY)

if(${DCMTK_VERSION} VERSION_GREATER_EQUAL 3.6.7)
    target_link_libraries(io_dimse PUBLIC DCMTK::dcmnet)
else()
    target_link_libraries(io_dimse PUBLIC dcmnet)
endif()

# original system dcmtk resources path
set(STORESCP "/etc/dcmtk/storescp.cfg")

# try various paths, depending of the package manager / platforms / ...
if(VCPKG_TARGET_TRIPLET)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND (EXISTS "${DCMTK_DIR}/../../debug${STORESCP}"))
        set(STORESCP "${DCMTK_DIR}/../../debug${STORESCP}")
    elseif(EXISTS "${DCMTK_DIR}/..${STORESCP}")
        set(STORESCP "${DCMTK_DIR}/..${STORESCP}")
    elseif(EXISTS "${DCMTK_DIR}/../..${STORESCP}")
        set(STORESCP "${DCMTK_DIR}/../..${STORESCP}")
    elseif(EXISTS "${DCMTK_DIR}/../../..${STORESCP}")
        set(STORESCP "${DCMTK_DIR}/../../..${STORESCP}")
    endif()
endif()

# Copy dcmtk resources
file(COPY "${STORESCP}" DESTINATION "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/io_dimse")

unset(STORESCP)

target_link_libraries(io_dimse PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
