set( NAME Tuto07GenericScene )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    module_guiQt                   # Start the module, load qt implementation of module_gui
    visuOgre                # Start the module, allow to use fwRenderOgre
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style
    flatIcon

    # Reader
    ioData
    ioVTK
    ioAtoms

    # Services
    uiIO
    uiImageQt

    # Generic Scene
    visuOgreAdaptor
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
        Tuto07GenericScene_AppCfg
) # Main application's configuration to launch
