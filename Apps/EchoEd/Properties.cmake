
set( NAME EchoEd )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    gui
    guiQt
    arDataReg
    dataReg
    servicesReg
    io
    ioVTK
    maths
    ioIGTL
    uiIGTL
    ioAtoms
    ioNetwork
    media
    mediaExt
    arMedia
    extDataReg
    preferences
    videoTools
    echoEdSimu
    ctrlCamp
    appXml
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTK
    visuVTKQt
    launcher
    videoQt
    patchMedicalData
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES EchoEdConfig)
