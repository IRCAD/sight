
set( NAME ioPacs )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( PRIORITY 1 )
set( DEPENDENCIES
    fwCore
    fwThread
    fwCom
    fwGui
    fwGuiQt
    fwPacsIO
    fwRuntime
    fwTools
    fwData
    fwDataTools
    fwMedData
    fwMedDataTools
    fwServices
    fwIO
    fwPreferences
    )
set( REQUIREMENTS
    dataReg
    servicesReg
    )
set( CONAN_DEPS
    ${CONAN_DCMTK}
    ${CONAN_QT}
)