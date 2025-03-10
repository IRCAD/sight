sight_add_target(ui_dicom TYPE LIBRARY)

find_package(DCMTK REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE DCMTK::dcmdata)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PUBLIC core data filter_dicom)
