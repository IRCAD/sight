fwLoadProperties()

find_package(VTK REQUIRED)

fwLink(
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonComputationalGeometry
    )
