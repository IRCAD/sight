fwLoadProperties()

find_package(DCMTK QUIET REQUIRED)
fwForwardInclude( ${DCMTK_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

if(UNIX)
    file(COPY "${DCMTK_DIR}/../../../share/dcmtk/dicom.dic"
         DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
else()
    file(COPY "${DCMTK_DIR}/../share/dcmtk/dicom.dic"
         DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")
endif()
