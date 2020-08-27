
set( NAME PoCStereo )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    fwMedData
    servicesReg
    gui
    guiQt
    ioData
    ioAtoms
    ioVTK
    uiIO
    uiTF
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

moduleParam(appXml PARAM_LIST config PARAM_VALUES PoCStereo)

