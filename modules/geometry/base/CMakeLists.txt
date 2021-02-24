sight_add_target( module_geometry_base TYPE MODULE )

find_package(glm QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      core
                      service
                      ui_base
                      geometry_data
)
