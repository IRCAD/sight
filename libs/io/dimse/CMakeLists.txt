fwLoadProperties()

find_package(DCMTK QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} PRIVATE ${DCMTK_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${DCMTK_LIBRARIES})

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
file(COPY
    "${STORESCP}"
    DESTINATION
    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}"
)

unset(STORESCP)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
)
