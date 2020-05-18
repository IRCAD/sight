
set( NAME uiMedDataQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwActivities
    fwCom
    fwCore
    fwData
    fwDataCamp
    fwDataTools
    fwGui
    fwGuiQt
    fwMedData
    fwMedDataTools
    fwRuntime
    fwServices
    fwTools
    fwDataTools
    fwIO
    )
set( REQUIREMENTS
    uiImageQt
    ctrlSelection
    )
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_QT}
)