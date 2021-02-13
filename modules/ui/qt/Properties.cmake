
set( NAME module_ui_qt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    activity
    core
    data
    geometry_data
    io_base
    service
    ui_qt
)
set( REQUIREMENTS
    module_ui_base
    module_data
)
set( PLUGINS
    platforms
    imageformats
    styles
    QtQml
    QtQuick
    QtQuick.2
)
