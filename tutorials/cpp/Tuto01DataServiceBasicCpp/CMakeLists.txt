sight_add_target(
    Tuto01DataServiceBasicCpp
    TYPE APP
    START ON
    WARNINGS_AS_ERRORS ON
)

add_dependencies(
    Tuto01DataServiceBasicCpp
    sightrun
    module_ui_qt
    module_service
    module_viz_sample
    module_io_vtk
    Tuto02DataServiceBasic # Needed for the sample image loaded at start
)

target_link_libraries(Tuto01DataServiceBasicCpp PUBLIC data service)

sight_generate_profile(Tuto01DataServiceBasicCpp)
