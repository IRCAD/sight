
set( NAME PoC07Registration )
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

    io
    ioData
    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm

    uiIO
    uiGenericQt
    uiMedDataQt
    appXml2
    activities
    ctrlSelection

    registrationActivity
    ioActivity

    media
    mediaExt

    patchMedicalData
    filterVRRender
    preferences
)

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES PoCRegistrationBase PoCRegistrationAppBase)
