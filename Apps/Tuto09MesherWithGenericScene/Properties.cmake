
set( NAME Tuto09MesherWithGenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    launcher
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioData
    ioVTK
    uiVisuQt
    uiImageQt
    uiReconstructionQt
    uiMedDataQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    opVTKMesh
    ctrlSelection
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Tuto09MesherWithGenericScene)
