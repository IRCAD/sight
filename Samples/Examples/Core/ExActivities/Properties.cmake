
set( NAME ExActivities )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    servicesReg
    dataReg

    gui
    guiQt

    fwlauncher
    appXml

    patchMedicalData
    filterUnknownSeries

    2DVisualizationActivity
    vtkSimpleNegato

    activities
    ioActivity

    ioAtoms
    ioVTK

    uiMedDataQt

    media

    preferences
    ctrlCamp
)

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES ExBase ExAppBase)
