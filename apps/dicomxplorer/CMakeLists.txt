sight_add_target(dicomxplorer TYPE APP)

add_dependencies(
    dicomxplorer
    module_service
    data
    module_ui_base
    module_ui_qt
    module_activity
    module_appXml
    module_memory
    sightrun
    module_io_itk
    module_io_vtk
    module_io_dicom
    module_io_matrix
    module_io_activity
    module_io_session
    activity_io_dicom
    activity_io_dicomweb
    activity_viz_negato
    activity_viz_3d
    module_ui_icons
    atoms
    module_ui_base
)

module_param(module_appXml PARAM_LIST config parameters PARAM_VALUES dicomXplorerBase dicomXplorerAppBase)

sight_generate_profile(dicomxplorer)
