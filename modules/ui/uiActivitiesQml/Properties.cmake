
set( NAME uiActivitiesQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    activities
    data
    ui_base
    fwQml
)
set( REQUIREMENTS
    styleQml
    data
    module_services
    module_io_atoms
    ioActivity
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
