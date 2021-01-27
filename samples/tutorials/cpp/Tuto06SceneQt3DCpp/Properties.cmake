set( NAME Tuto06SceneQt3DCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    data
    services
)
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher

    preferences             # Start the module, load file location or window preferences
    module_guiQt                   # Start the module, load qt implementation of module_gui
    visuQt3DQt              # Enable Qt3D to render things in Qt window

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

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto14MeshGenerator_AppCfg
) # Main application's configuration to launch
