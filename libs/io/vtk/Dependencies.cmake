find_package(
    VTK QUIET REQUIRED
    COMPONENTS vtkFiltersGeometry
               vtkFiltersModeling
               vtkIOExport
               vtkIOGeometry
               vtkIOImage
               vtkIOLegacy
               vtkIOPLY
               vtkIOXML
               vtkRenderingFreeType
               vtkRenderingOpenGL2 # Needed notably for the VtkObjExporter
)
