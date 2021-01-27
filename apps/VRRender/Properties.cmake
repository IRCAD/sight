
set( NAME VRRender )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    module_services
    data

    module_gui
    module_guiQt

    activities
    appXml
    memory
    fwlauncher

    ioAtoms
    ioITK
    ioVTK
    ioGdcm
    ioData

    uiIO
    uiGenericQt
    uiMedDataQt
    uiPreferences
    uiActivitiesQt

    2DVisualizationActivity
    3DVisualizationActivity
    volumeRenderingActivity
    blendActivity
    ioActivity

    dicomFilteringActivity
    dicomPacsReaderActivity
    dicomPacsWriterActivity

    DicomWebReaderActivity
    DicomWebWriterActivity

    media
    style

    patchMedicalData
    arPatchMedicalData

    filterUnknownSeries

    preferences
)

moduleParam(appXml PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
