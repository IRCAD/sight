set( NAME Tuto16SceneQt3D )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_guiQt                   # Start the module, allow dark theme

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style
    uiIO

    # Reader
    ioVTK
    ioAtoms

    # Generic Scene
    visuQt3DAdaptor
    visuQt3DQt
)

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt

bundleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto16SceneQt3D_AppCfg
) # Main application's configuration to launch
