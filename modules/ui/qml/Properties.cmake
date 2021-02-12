
set( NAME module_ui_qml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    ui_qml
    module_ui_qt
    services
)
set( REQUIREMENTS
    module_ui_base
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
    platforms
    imageformats
)
