
set( NAME ExActivities )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    servicesReg
    fwData

    gui
    guiQt

    fwlauncher
    appXml

    2DVisualizationActivity
    volumeRenderingActivity
    vtkSimpleNegato

    activities
    ioActivity
    activitiesConfig

    ioAtoms
    ioVTK

    uiIO

    media

    preferences
    ctrlCamp
)

moduleParam(appXml PARAM_LIST config parameters PARAM_VALUES ExBase ExAppBase)
