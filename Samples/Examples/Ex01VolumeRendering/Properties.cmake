
set( NAME Ex01VolumeRendering )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlCamp
    ctrlSelection
    gui
    guiQt
    ioData
    ioAtoms
    ioVTK
    io
    servicesReg
    uiVisuQt
    uiImageQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKVRAdaptor
    fwlauncher
    appXml2
    scene2D
    uiTF
    preferences
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex01VolumeRendering)
