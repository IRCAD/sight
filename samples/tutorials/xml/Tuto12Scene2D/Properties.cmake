set( NAME Tuto12Scene2D )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_guiQt                   # Start the module, load qt implementation of module_gui

    # Objects declaration
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style

    # Generic Scene
    scene2D
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
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto12Scene2D_AppCfg
   ) # Allow dark theme via module_guiQt
