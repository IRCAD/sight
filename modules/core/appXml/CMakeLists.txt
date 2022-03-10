sight_add_target(
    module_appXml
    TYPE MODULE
    START ON
    PRIORITY 0
) # we give the lowest priority because the AppXml needs to be launched last

add_dependencies(module_appXml module_service)

target_link_libraries(module_appXml PUBLIC core service)
