
set( NAME EchoEd )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    arDataReg
    dataReg
    servicesReg

    io
    ioVTK
    ioIGTL
    ioAtoms
    ioNetwork

    gui
    guiQt

    media
    arMedia

    echoEdSimu
    maths

    ctrlCamp

    videoQt
    videoTools

    appXml
    fwlauncher
    patchMedicalData
    preferences
    uiPreferences

    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt

    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES EchoEdConfig)
