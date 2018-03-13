fwLoadProperties()

find_package(VTK QUIET
            COMPONENTS 
            vtkCommonCore 
            vtkCommonDataModel 
            vtkCommonComputationalGeometry
            NO_MODULE 
            REQUIRED)

fwForwardInclude( ${VTK_INCLUDE_DIRS} )
fwLink( ${VTK_LIBRARIES} )
