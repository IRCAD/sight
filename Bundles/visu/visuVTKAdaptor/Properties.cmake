
set( NAME visuVTKAdaptor )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataCamp
    fwGui
    fwMath
    fwMedData
    fwRenderVTK
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
)
set( REQUIREMENTS
    dataReg
    servicesReg
    visuVTK
)
set( USE_PCH_FROM_TARGET fwRenderVTK )
