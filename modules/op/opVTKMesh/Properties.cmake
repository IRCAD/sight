
set( NAME opVTKMesh )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES 
    fwMedData
    fwCom
    fwData
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
    fwCore
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_VTK}
)