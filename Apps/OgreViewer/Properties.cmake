
set( NAME OgreViewer )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlSelection
    ctrlCamp
    gui
    guiQt
    ioData
    ioAtoms
    io
    ioVTK
    servicesReg
    uiVisuQt
    uiImageQt
    uiReconstructionQt
    uiMedDataQt
    uiIO
    uiTF
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml2
    arMedia
    arDataReg
    videoQt
    videoTools
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiVisuOgre
    scene2D
    material
    media
    preferences
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES OgreViewer_Extension)
