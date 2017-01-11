
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

    uiIGTL

    media
    arMedia

    echoEdSimu
    maths

    ctrlCamp

    videoQt
    videoTools

    appXml2
    fwlauncher
    patchMedicalData
    preferences

    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt

    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES EchoEdConfig)
