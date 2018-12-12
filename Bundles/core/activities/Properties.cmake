
set( NAME activities )
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
    qt/5.11.2@sight/stable
)