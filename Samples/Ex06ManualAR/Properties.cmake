
set( NAME Ex06ManualAR )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    activities
    appXml2
    arDataReg
    arMedia
    ctrlCamp
    dataReg
    fwlauncher
    gui
    guiQt
    ioCalibration
    uiPreferences
    uiTools
    maths
    preferences
    servicesReg
    trackerAruco
    registrationARL
    videoQt
    videoTools
    videoCalibration
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt
    fwVtkIO
    ioData
    uiIO
    ioVTK
    ioAtoms
    3DVisualizationActivity2
    uiMedDataQt

    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex06ManualARConfig)
