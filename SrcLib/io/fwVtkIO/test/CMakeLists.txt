fwLoadProperties()
find_package(CppUnit)
find_package(VTK REQUIRED)

fwForwardInclude(
    ${CPPUNIT_INCLUDE_DIR}
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${CPPUNIT_LIBRARY}
    vtkIOLegacy
    vtkFiltersSources
    vtkCommonExecutionModel
    vtkCommonDataModel
    vtkCommonCore
)
