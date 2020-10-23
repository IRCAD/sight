set( NAME Tuto08GenericSceneCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    fwData
    fwRuntime
    fwServices
)
set( REQUIREMENTS
    preferences             # Start the module, load file location or window preferences
    guiQt                   # Start the module, allow dark theme
    visuOgre                # Start the module, allow to use fwRenderOgre
    material                # Start the module, load Ogre's materials
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    servicesReg             # fwService

    # UI declaration/Actions
    gui
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
set( WARNINGS_AS_ERRORS ON )

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt
