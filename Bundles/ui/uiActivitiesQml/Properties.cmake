
set( NAME uiActivitiesQml )
set( VERSION 0.1 )
set( TYPE BUNDLE )
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
    dataReg
    servicesReg
    ioAtoms
    ioActivity
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)

set( CONAN_DEPS
    ${CONAN_QT}
)
