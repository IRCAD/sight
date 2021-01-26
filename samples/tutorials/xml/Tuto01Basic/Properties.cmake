set( NAME Tuto01Basic )     # Name of the application
set( VERSION 0.2 )          # Version of the application
set( TYPE APP )             # Type APP represent "Application"
set( DEPENDENCIES  )        # For an application we have no dependencies (libraries to link)
set( REQUIREMENTS           # The modules used by this application
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations
    guiQt                   # Start the module, load qt implementation of gui

    module_services             # fwService

    # UI declaration/Actions
    gui
)

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto01Basic_AppCfg
) # Main application's configuration to launch
