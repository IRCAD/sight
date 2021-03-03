set( NAME Tuto04SignalSlot )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    sightrun                     # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_qt                   # Start the module, load module_ui_qt implementation of module_ui_base

    # Objects declaration
    data
    module_service             

    # UI declaration/Actions
    module_ui_base

    # Reader
    module_io_vtk

    # Services
    module_viz_sample
)

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
        Tuto04SignalSlot_AppCfg
) # Main application's configuration to launch
