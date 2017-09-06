
set( NAME Tuto05Mesher )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioVTK
    visuVTKQt
    uiIO
    vtkSimpleNegato
    vtkSimpleMesh
    opVTKMesh # provides services to generate a mesh from an image.
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES MesherConfig)
