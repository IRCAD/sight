sight_target( module_filter_point TYPE MODULE )



find_package(VTK QUIET COMPONENTS vtkCommonDataModel REQUIRED)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PRIVATE VTK::CommonDataModel)
else()
    include(${VTK_USE_FILE})
    target_link_libraries(${FWPROJECT_NAME} PRIVATE vtkCommonDataModel)
endif()

find_package(ITK QUIET COMPONENTS ITKCommon REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ITKCommon)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      module_ui_qt
                      io_itk
                      ui_base
)
