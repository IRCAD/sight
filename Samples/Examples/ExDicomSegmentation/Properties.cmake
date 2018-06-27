set( NAME ExDicomSegmentation )
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
    ioGdcm
    ioVTK
    uiIO
    uiVisuQt
    uiReconstructionQt
    uiMedDataQt
    visuVTKAdaptor
    visuVTKQt
    opVTKMesh
    opImageFilter
    fwlauncher
    appXml
    activities
    media
    ioActivity
    2DVisualizationActivity
    3DVisualizationActivity
    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExDicomSegmentationAppConfigBase)
