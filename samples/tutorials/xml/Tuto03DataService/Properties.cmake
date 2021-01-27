set( NAME Tuto03DataService )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_guiQt                   # Start the module, load qt implementation of module_gui

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui

    # Reader
    ioVTK

    # Services
    uiIO                    # Contains services to show dialogs for reader/writer selection
    visuBasic
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto03DataService_AppCfg
) # Main application's configuration to launch
