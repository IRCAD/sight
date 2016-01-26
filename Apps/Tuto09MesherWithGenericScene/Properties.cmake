
set( NAME Tuto09MesherWithGenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioData
    ioVTK
    uiIO
    uiVisuQt
    uiImageQt
    uiReconstructionQt # contains editors to manage reconstructions
    uiMedDataQt # contains editors to manage model series
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    opVTKMesh
    ctrlSelection
    launcher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Tuto09MesherWithGenericScene)
