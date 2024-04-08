sight_add_target(ex_dicom_segmentation TYPE APP UNIQUE ON)

add_dependencies(
    ex_dicom_segmentation
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    module_activity
    data
    module_service
    module_ui
    module_ui_icons
    module_io_activity
    module_io_matrix
    module_io_dicom
    module_io_vtk
    module_io_bitmap
    activity_viz_negato
    module_filter_image
    module_data
    module_filter_mesh
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExDicomSegmentation_AppCfg)

sight_generate_profile(ex_dicom_segmentation)
