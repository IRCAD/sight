
set( NAME VRViewer )
set( VERSION 0.3 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlSelection
    ctrlCamp
    gui
    guiQt
    io
    ioActivity
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
    patchMedicalData
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES VRViewer_Extension)
