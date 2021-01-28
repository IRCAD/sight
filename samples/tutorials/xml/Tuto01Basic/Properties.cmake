set( NAME Tuto01Basic )     # Name of the application
set( VERSION 0.2 )          # Version of the application
set( TYPE APP )             # Type APP represent "Application"
set( DEPENDENCIES  )        # For an application we have no dependencies (libraries to link)
set( REQUIREMENTS           # The modules used by this application
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations
    module_ui_qt            # Start the module, load qt implementation of module_ui_base

    module_services

    # UI declaration/Actions
    module_ui_base
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto01Basic_AppCfg
) # Main application's configuration to launch
