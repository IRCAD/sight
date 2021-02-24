sight_add_target( module_io_pcl TYPE MODULE )

find_package(PCL QUIET REQUIRED COMPONENTS common io)
target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${VTK_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES})

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      ui_base
)
