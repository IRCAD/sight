
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
    ioData
    ioVTK
    uiIO
    uiVisuQt
    uiReconstructionQt # contains editors to manage reconstructions
    uiMedDataQt # contains editors to manage model series
    visuVTKAdaptor
    visuVTKQt
    opVTKMesh
    fwlauncher
    appXml
    ctrlCamp
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Tuto09MesherWithGenericScene)
