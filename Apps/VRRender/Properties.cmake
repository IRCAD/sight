
set( NAME VRRender )
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
    visuVTKVRAdaptor
    gui
    guiQt
    memory
    monitor
    monitorQt
    ctrlMemory
    launcher
    io
    ioVTK
    ioVtkGdcm
    uiIO
    uiGenericQt
    uiMedDataQt
    uiImageQt
    uiVisu
    uiVisuQt
    appXml
    activities
    ctrlSelection
    uiReconstructionQt
    2DVisualizationActivity
    3DVisualizationActivity
    blendActivity
    volumeRenderingActivity
    media
    ioAtoms
    patchMedicalData
    ioITK
    filterVRRender
    scene2D
    uiMeasurement
    uiMeasurementQt
    uiTF
)

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
