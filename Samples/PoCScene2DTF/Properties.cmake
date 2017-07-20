
set( NAME PoCScene2DTF)
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    io
    fwlauncher
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    ioVTK
    ioData
    uiImageQt
    uiVisuQt
    ctrlSelection
    ioAtoms
    appXml
    scene2D
    uiTF
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoScene2DTFConfig)
