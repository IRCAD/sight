sight_add_target(module_io_dicom_ut TYPE DOCTEST)

target_sources(module_io_dicom_ut PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../reader.cpp)
target_include_directories(module_io_dicom_ut PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(
    module_io_dicom_ut
    PRIVATE utest_data
            data
            io
            io_dicom
            service
            ui
            ui_qt
            ui_test
)
