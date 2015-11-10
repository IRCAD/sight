
set( NAME Ex01VolumeRendering )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    dataReg
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
    visu
    launcher
    appXml
    scene2D
    uiTF
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex01VolumeRendering)
