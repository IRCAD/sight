
set( NAME Tuto09MesherWithGenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    fwMedData
    servicesReg
    gui
    guiQt
    ioData
    ioVTK
    ctrlSelection
    uiIO
    uiVisuQt
    uiImageQt
    uiReconstructionQt # contains editors to manage reconstructions
    uiMedDataQt # contains editors to manage model series
    visuVTKAdaptor
    visuVTKQt
    opVTKMesh
    fwlauncher
    appXml
    ctrlCamp
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES Tuto09MesherWithGenericScene)
