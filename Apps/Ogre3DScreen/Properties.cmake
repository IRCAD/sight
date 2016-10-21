
set( NAME Ogre3DScreen )
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
    fwlauncher
    appXml2
    arMedia
    arDataReg
    visuOgre
    visuOgreQt
    visuVTKQt
    visuOgreAdaptor
    uiVisuOgre
    material
    media
    preferences
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ogre3DScreen_Extension)
