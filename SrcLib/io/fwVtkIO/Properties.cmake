
set( NAME fwVtkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataIO
    fwMath
    fwMedData
    fwMemory
    fwTools
    fwJobs
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    vtk/8.0.1@sight/stable
)