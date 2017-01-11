
set( NAME Tuto13Scene2D )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    uiVisu
    uiVisuQt
    scene2D
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoScene2DConfig)
