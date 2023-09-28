sight_add_target(ui_dicom TYPE LIBRARY)

find_package(DCMTK REQUIRED)

if(${DCMTK_VERSION} VERSION_GREATER_EQUAL 3.6.7)
    target_link_libraries(ui_dicom PRIVATE DCMTK::dcmdata)
else()
    target_link_libraries(ui_dicom PRIVATE dcmdata)
endif()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(ui_dicom PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(ui_dicom PROPERTIES AUTOMOC TRUE)

target_link_libraries(ui_dicom PUBLIC core data filter_dicom ui ui_qt)
