sight_add_target( module_io_pcl TYPE MODULE )

find_package(PCL QUIET REQUIRED COMPONENTS common io)
target_include_directories(module_io_pcl SYSTEM PRIVATE ${VTK_INCLUDE_DIRS})
target_link_libraries(module_io_pcl PRIVATE ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES})

target_link_libraries(module_io_pcl PUBLIC 
                      core
                      data
                      service
                      ui_base
)
