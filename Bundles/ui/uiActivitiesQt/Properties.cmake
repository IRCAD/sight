
set( NAME uiActivitiesQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwActivities
    fwCom
    fwData
    fwDataCamp
    fwGui
    fwGuiQt
    fwMedData
    fwRuntime
    fwServices
    fwTools
    fwMedDataTools
)
set( REQUIREMENTS
    guiQt
    dataReg
    servicesReg
)

set( CONAN_DEPS
    ${CONAN_QT}
)
