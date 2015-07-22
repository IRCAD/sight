add_definitions(-DPRJ_NAME=\"${NAME}\")
set( NAME ioDicomExt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    gui
    io
    fwCore
    fwCom
    fwTools
    fwData
    fwServices
    fwMedData
    fwGui
    fwGuiQt
    fwDicomIOExt
    fwDicomIOFilter
    fwDicomIOFilterQt
    fwDicomData
    fwComEd
    fwDataIO
    fwRuntime
    fwThread
    )
