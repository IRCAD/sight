sight_add_target(ex_image_reslice TYPE APP)

add_dependencies(
    ${SIGHT_TARGET}
    sightrun
    module_app
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3d_qt
    data
    module_service
    module_geometry
    module_ui
    module_ui_icons
    module_io_dicom
    module_io_vtk
    module_io_itk
    module_filter_image
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES sight::examples::filter::ex_image_reslice::config)
