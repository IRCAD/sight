set( NAME Tuto02GenericSceneCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    data
    services
)
set( REQUIREMENTS
    preferences             # Start the module, load file location or window preferences
    module_guiQt                   # Start the module, allow dark theme
    visuOgre                # Start the module, allow to use fwRenderOgre
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
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
set( WARNINGS_AS_ERRORS ON )

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt
