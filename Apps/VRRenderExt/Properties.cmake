
set( NAME VRRenderExt )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS
    servicesReg
    dataReg
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKVRAdaptor
    gui
    guiQt
    guiQtExt
    memory
    monitor
    monitorQt
    ctrlMemory
    launcher
    ioDicom
    io
    ioGdcm
    ioDcmtk
    ioPacs
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
    dicomPacsReaderActivity
    dicomPacsWriterActivity
    dicomFilteringActivity
    dicomAppConfig
    media
    ioAtoms
    patchMedicalData
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
    scene2D
    uiVisu
    uiVisuQt
    uiMeasurement
    uiMeasurementQt
    uiTF
    filterVRRender
)

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES VRRenderExtBase VRRenderExtAppBase)
