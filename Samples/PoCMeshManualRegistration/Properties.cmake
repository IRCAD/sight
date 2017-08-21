
set( NAME PoCMeshManualRegistration)
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    io
    ioITK
    fwlauncher
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    ioVTK
    ioData
    uiImageQt
    uiVisuQt
    ctrlSelection
    uiReconstructionQt
    uiMedDataQt
    appXml2
    opVTKMesh
    media
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES PoCMeshManualRegistration)
