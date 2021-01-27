set( NAME ExRealSense )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    module_guiQt                   # Start the module, allow dark theme
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style
    flatIcon

    # Grabber
    videoQt
    videoRealSense

    # Services
    videoTools
    uiTools
    ctrlCamp

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
    ExRealSense_AppCgf
) # Main application's configuration to launch

