sight_add_target( module_geometry_base TYPE MODULE WARNINGS_AS_ERRORS OFF)

find_package(glm QUIET REQUIRED)
target_include_directories(module_geometry_base SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

add_dependencies(module_geometry_base 
                 data
                 module_service
)

target_link_libraries(module_geometry_base PUBLIC 
                      data
                      core
                      service
                      ui_base
                      geometry_data
)
