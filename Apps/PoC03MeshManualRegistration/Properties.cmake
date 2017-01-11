
set( NAME PoC03MeshManualRegistration)
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
    appXml
    opVTKMesh
    media
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoC03MeshManualRegistration)
