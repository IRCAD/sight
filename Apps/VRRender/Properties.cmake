
set( NAME VRRender )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKVRAdaptor
    gui
    guiQt
    memory
    ctrlMemory
    fwlauncher
    io
    ioVTK
    ioVtkGdcm
    ioData
    uiIO
    uiGenericQt
    uiMedDataQt
    uiImageQt
    uiVisu
    uiVisuQt
    appXml2
    activities
    ctrlSelection
    uiReconstructionQt
    2DVisualizationActivity2
    3DVisualizationActivity2
    volumeRenderingActivity2
    blendActivity
    ioActivity
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

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
