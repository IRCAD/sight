
set( NAME Tuto13Scene2D )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    io
    appXml
    gui
    guiQt
    dataReg
    servicesReg
    launcher
    scene2D
    uiVisu
    uiVisuQt
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoScene2DConfig)
