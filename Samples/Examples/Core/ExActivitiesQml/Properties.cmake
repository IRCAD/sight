
set( NAME ExActivitiesQml )
set( VERSION 0.1 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    fwRuntime
    fwData
    fwQml
)
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQml
    activities
    fwlauncher
    visuVTKQml
    visuVTKAdaptor
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
