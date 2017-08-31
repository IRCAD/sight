
set( NAME Ex05Activities )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    servicesReg
    dataReg

    gui
    guiQt

    fwlauncher
    appXml2

    patchMedicalData
    filterVRRender

    2DVisualizationActivity
    vtkSimpleNegato

    activities
    ioActivity

    ioAtoms
    ioVTK

    uiMedDataQt
    ctrlMemory

    media

    preferences
    ctrlCamp
)

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES Ex05Base Ex05AppBase)
