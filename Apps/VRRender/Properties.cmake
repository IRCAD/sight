
set( NAME VRRender )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg

    gui
    guiQt

    activities
    appXml2
    memory
    ctrlMemory
    fwlauncher

    io
    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm
    ioData

    uiIO
    uiGenericQt
    uiMedDataQt

    2DVisualizationActivity2
    3DVisualizationActivity2
    volumeRenderingActivity2
    blendActivity
    ioActivity

    dicomFilteringActivity
    dicomPacsReaderActivity
    dicomPacsWriterActivity

    media

    patchMedicalData

    filterVRRender

    preferences
)

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
