
set( NAME ioDicom )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    fwDicomIOFilter
    fwDicomIOFilterQt
    fwGui
    fwGuiQt
    fwJobs
    fwLog
    fwRuntime
    fwServices
    fwIO
    fwData
    fwDataTools
    fwMedData
    fwMedDataTools
    )
set( REQUIREMENTS
    fwData
    servicesReg
)