set( NAME TutoEditorQml )
set( VERSION 0.1 )
set( TYPE APP )
set( START ON)
set( DEPENDENCIES
    fwRuntime
    fwData
    fwServices
    fwMedData
    fwQml
)
set( REQUIREMENTS
    dataReg
    servicesReg
    guiQt
    fwlauncher
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
