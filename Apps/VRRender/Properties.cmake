
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
    style

    patchMedicalData

    filterVRRender

    preferences
)

bundleParam(guiQt 
    PARAM_LIST 
        resource
        stylesheet 
    PARAM_VALUES 
        ${BUNDLE_PREFIX}/style_0-1/darkstyle.rcc 
        ${BUNDLE_PREFIX}/style_0-1/darkstyle.qss
)
bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
