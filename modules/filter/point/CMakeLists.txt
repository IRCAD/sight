sight_add_target( module_filter_point TYPE MODULE)

find_package(VTK QUIET COMPONENTS vtkCommonDataModel REQUIRED)

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(module_filter_point SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(module_filter_point PRIVATE vtkCommonDataModel)
else()
    target_link_libraries(module_filter_point PRIVATE VTK::CommonDataModel)
endif()

find_package(ITK QUIET COMPONENTS ITKCommon REQUIRED)
target_link_libraries(module_filter_point PRIVATE ITKCommon)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_filter_point PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(module_filter_point PROPERTIES AUTOMOC TRUE)

target_link_libraries(module_filter_point PUBLIC
                      data
                      ui_qt
                      io_itk
                      ui_base
)
