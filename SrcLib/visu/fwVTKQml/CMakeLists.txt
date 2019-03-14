fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
find_package(VTK QUIET COMPONENTS vtkGUISupportQt vtkRenderingExternal REQUIRED)

fwInclude(
    ${VTK_INCLUDE_DIRS}
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Qml Qt5::Quick vtkGUISupportQt vtkRenderingExternal)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
