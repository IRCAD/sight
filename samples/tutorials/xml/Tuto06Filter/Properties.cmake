set( NAME Tuto06Filter )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    module_guiQt                   # Start the module, load qt implementation of module_gui

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style

    # Reader
    ioVTK

    # Services
    uiIO
    visuBasic
    opImageFilter # module containing the action to performs a threshold
)

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto06Filter_AppCfg
) # Main application's configuration to launch
