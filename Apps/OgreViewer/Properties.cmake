
set( NAME OgreViewer )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlSelection
    gui
    guiQt
    ioData
    ioAtoms
    io
    ioVTK
    servicesReg
    uiImageQt
    uiReconstructionQt
    uiMedDataQt
    uiIO
    uiTF
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml
    activities
    arMedia
    arDataReg
    videoQt
    videoTools
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiVisuOgre
    material
    media
    preferences
    ogreConfig
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES OgreViewer_Extension)
