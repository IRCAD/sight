
set( NAME PoCRegistration )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS
    servicesReg
    dataReg

    gui
    guiQt
    memory
    fwlauncher

    ioData
    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm

    uiIO
    uiGenericQt
    uiMedDataQt
    appXml
    activities
    ctrlSelection

    registrationActivity
    ioActivity

    media

    patchMedicalData
    filterUnknownSeries
    preferences
)

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES PoCRegistrationBase PoCRegistrationAppBase)
