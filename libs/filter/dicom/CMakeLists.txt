sight_target( filter_dicom TYPE LIBRARY PCH pchData )


fwForwardInclude( ${DCMTK_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      geometry_data
)
