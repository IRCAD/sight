
set( NAME vtkGdcmIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataIO
    fwMedData
    fwMemory
    fwTools
    fwVtkIO
    fwJobs
)
set( REQUIREMENTS )
set( CONAN_DEPS
    vtk/8.0.1@sight/stable
    gdcm/2.8.6@sight/stable
)