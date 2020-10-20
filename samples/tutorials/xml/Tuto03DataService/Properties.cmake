set( NAME Tuto03DataService )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    guiQt                   # Start the module, load qt implementation of gui

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui

    # Reader
    ioVTK

    # Services
    uiIO # contains services to show dialogs for reader/writer selection
    visuBasic
)

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto03DataService_AppCfg
) # Main application's configuration to launch
