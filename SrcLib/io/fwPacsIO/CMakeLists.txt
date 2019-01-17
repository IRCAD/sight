fwLoadProperties()

find_package(DCMTK QUIET REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIRS} )
fwForwardLink( ${DCMTK_LIBRARIES} )

if(UNIX)
    file(COPY "${DCMTK_DIR}/../../../etc/dcmtk/storescp.cfg"
         DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
else()
    file(COPY "${DCMTK_DIR}/../etc/dcmtk/storescp.cfg"
         DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
endif()
