
set( NAME PoCDialog )
set( VERSION 0.1 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    fwData
    fwMedData
    fwDataTools
    fwServices
    fwCom
    fwRuntime
    fwVTKQml
)
set( REQUIREMENTS
    ctrlCamp
    dataReg
    gui
    guiQml
    ioData
    ioVTK
    servicesReg
    opVTKMesh
    uiIO
    uiImageQml
    visuVTKAdaptor
    visuVTKQml
    fwlauncher
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
