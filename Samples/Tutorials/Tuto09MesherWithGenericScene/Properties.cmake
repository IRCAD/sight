
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
    uiReconstructionQt # contains editors to manage reconstructions
    uiMedDataQt # contains editors to manage model series
    visuVTKAdaptor
    visuVTKQt
    opVTKMesh
    fwlauncher
    appXml2
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Tuto09MesherWithGenericScene)
