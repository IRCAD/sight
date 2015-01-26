add_definitions(-DPRJ_NAME=\"${NAME}\")
set( NAME ioDicomExt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    gui
    io
    fwCore
    fwServices
    fwMedData
    fwGui
    fwGuiQt
    fwDicomIOExt
    fwDicomIOFilterQt
    fwDicomData
    )
