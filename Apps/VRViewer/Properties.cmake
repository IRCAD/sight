
set( NAME VRViewer )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlSelection
    ctrlCamp
    gui
    guiQt
    io
    ioData
    ioAtoms
    ioDicom
    ioVtkGdcm
    servicesReg
    fwlauncher
    appXml2
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiIO
    uiMedDataQt
    uiTF
    uiVisuOgre
    uiVisuQt
    scene2D
    material
    media
    preferences
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES VRViewer_Extension)
