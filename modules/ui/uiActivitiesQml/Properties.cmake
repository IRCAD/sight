
set( NAME uiActivitiesQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCore
    fwActivities
    fwCom
    fwData
    fwGui
    fwQml
)
set( REQUIREMENTS
    styleQml
    fwData
    servicesReg
    ioAtoms
    ioActivity
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)