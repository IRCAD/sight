
set( NAME uiActivitiesQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    activities
    data
    gui
    fwQml
)
set( REQUIREMENTS
    styleQml
    data
    module_services
    ioAtoms
    ioActivity
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
