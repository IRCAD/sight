sight_add_target( ui_dicom TYPE LIBRARY PCH pchData )

find_package(DCMTK REQUIRED)
target_include_directories(ui_dicom SYSTEM PRIVATE ${DCMTK_INCLUDE_DIRS})
target_link_libraries(ui_dicom PRIVATE ${DCMTK_LIBRARIES})

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(ui_dicom PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(ui_dicom PROPERTIES AUTOMOC TRUE)

target_link_libraries(ui_dicom PUBLIC 
                      core
                      data
                      filter_dicom
                      ui_base
                      ui_qt
)
