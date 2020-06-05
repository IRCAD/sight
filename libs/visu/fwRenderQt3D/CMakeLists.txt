fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS
    Core
    Gui
    Widgets
    Quick
    3DCore
    3DRender
    3DExtras
    3DInput
REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Quick
    Qt5::3DCore
    Qt5::3DRender
    Qt5::3DExtras
    Qt5::3DInput
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)