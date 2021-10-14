sight_add_target(Tuto02GenericSceneCpp TYPE APP START ON)

add_dependencies(Tuto02GenericSceneCpp
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_matrix
                 module_io_vtk
)

target_link_libraries(Tuto02GenericSceneCpp PUBLIC data service)

# Set dark theme via module_ui_qt
moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
)

sight_generate_profile(Tuto02GenericSceneCpp)
