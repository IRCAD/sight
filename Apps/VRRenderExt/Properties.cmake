
set( NAME VRRenderExt )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS
    servicesReg
    dataReg

    gui
    guiQt
    memory
    monitor
    monitorQt
    ctrlMemory
    fwlauncher

    io
    ioData
    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm

    uiIO
    uiGenericQt
    uiMedDataQt
    appXml2
    activities
    ctrlSelection
    uiReconstructionQt

    2DVisualizationActivity
    3DVisualizationActivity
    volumeRenderingActivity
    blendActivity
    ioActivity
    dicomFilteringActivity
    dicomPacsReaderActivity
    dicomPacsWriterActivity
    cprActivity
    mesherActivity

    media
    mediaExt

    patchMedicalData
    filterVRRender
    preferences
)

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES VRRenderExtBase VRRenderExtAppBase)
