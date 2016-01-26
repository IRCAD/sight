
fwLoadProperties()
fwUseForwardInclude(
    gui
    fwData
    fwServices
    fwComEd
    fwDataTools
    fwGuiQt
    fwItkIO
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(ITK REQUIRED)
find_package(VTK REQUIRED)
find_library(VGL_ALGO_LIBRARY   vgl_algo )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(VNL_LIBRARY        vnl )
find_library(VCL_LIBRARY        vcl )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )
find_path(VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)

fwForwardInclude(
    ${ITK_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    
    ${VGL_ALGO_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    
    ITKCommon

    vtkCommonDataModel
    vtkFiltersCore
    vtkFiltersGeometry
    vtkFiltersGeneral
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
