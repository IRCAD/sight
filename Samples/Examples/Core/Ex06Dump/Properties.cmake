
set( NAME Ex06Dump )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg

    gui
    guiQt

    appXml
    memory
    fwlauncher

    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm
    ioData

    uiIO
    uiMedDataQt

    2DVisualizationActivity
    3DVisualizationActivity
    ioActivity

    media

    patchMedicalData

    filterVRRender

    preferences

    monitor
    monitorQt
)

# Set the reading mode to "lazy", so the buffers will only be read in memory when they are used.
bundleParam(memory PARAM_LIST loading_mode PARAM_VALUES lazy)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex06DumpConfig)
