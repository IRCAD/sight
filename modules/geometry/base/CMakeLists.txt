sight_target( module_geometry_base TYPE MODULE )


find_package(glm QUIET REQUIRED)

fwInclude(

    ${GLM_INCLUDE_DIRS}
)

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
