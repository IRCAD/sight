
set( NAME PoCStereo )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioData
    ioAtoms
    ioVTK
    uiIO
    fwlauncher
    appXml
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiVisuOgre
    material
    media
    preferences
    scene2D
    qtSceneConfig
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoCStereo)

