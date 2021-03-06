sight_add_target( ExDump TYPE APP )


add_dependencies(ExDump 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_atoms
                 module_io_itk
                 module_io_vtk
                 module_io_dicom
                 module_io_matrix
                 module_memory
                 activity_viz_negato
                 module_io_activity
                 atoms
                 module_debug
                 module_ui_debug
)

moduleParam(
        module_memory
    PARAM_LIST
        loading_mode
    PARAM_VALUES
        lazy
) # Set the reading mode to "lazy", so the buffers will only be read in module_memory when they are used.
moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExDump_AppCfg
) # Main application's configuration to launch

sight_generate_profile(ExDump)
