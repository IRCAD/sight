
set( NAME ioDicom )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwDicomIOFilter
    fwDicomIOFilterQt
    fwGui
    fwGuiQt
    fwJobs
    fwLog
    fwRuntime
    fwServices
    fwTools
    fwIO
    fwData
    fwDataTools
    fwMedData
    fwMedDataTools
    fwThread
    )
set( REQUIREMENTS
    dataReg
    servicesReg
    )
set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
)