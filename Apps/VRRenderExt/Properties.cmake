
set( NAME VRRenderExt )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    visu
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    gui
    guiQt
    memory
    monitor
    monitorQt
    ctrlMemory
    launcher
    ioGdcm
    ioITK
    ioVTK
    ioVtkGdcm
    uiIO
    uiGenericQt
    uiMedDataQt
    uiImageQt
    appXml
    activities
    ctrlSelection
    uiReconstructionQt
    2DVisualizationActivity
    3DVisualizationActivity
    blendActivity
    volumeRenderingActivity
    dicomActivity
    dicomReaderActivity
    media
    ioAtoms
    patchMedicalData
    ioDicomExt
    mediaExt
    uiViewPoints
    uiNavigation
    uiSpline
    uiCPR
    ctrlSplineNavigation
    ctrlPointSelection
    ctrlComputeCPR
    visuNavigation
    cprActivity
    viewPointsActivity
    splineEditionActivity
)

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES VRRenderExtBase VRRenderExtAppBase)