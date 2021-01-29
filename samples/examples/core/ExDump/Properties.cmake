set( NAME ExDump )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    module_ui_qt                   # Start the module, allow dark theme

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    style
    flatIcon

    # Reader
    module_io_atoms
    ioITK
    module_io_vtk
    ioGdcm
    module_io_file

    # Services
    memory
    uiIO
    uiMedDataQt
    2DVisualizationActivity
    3DVisualizationActivity
    ioActivity
    patchMedicalData
    filterUnknownSeries
    monitor
    monitorQt
)

moduleParam(
        memory
    PARAM_LIST
        loading_mode
    PARAM_VALUES
        lazy
) # Set the reading mode to "lazy", so the buffers will only be read in memory when they are used.

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExDump_AppCfg
) # Main application's configuration to launch
