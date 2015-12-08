
set( NAME Tuto05MesherB )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    io
    gui
    guiQt
    dataReg
    servicesReg
    visuVTK
    visuVTKQt
    launcher
    ioVTK
    ioData
    vtkSimpleNegato
    vtkSimpleMesh
    vtkCompositeMesh
    uiIO
    opVTKMesh
    opImageFilter
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES MesherV2Config)
