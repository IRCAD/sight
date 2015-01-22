fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwCore
    fwData
    fwDataIO
    fwTools
)

find_package(ITK REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED )

find_library(VGL_ALGO_LIBRARY   vgl_algo )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(VNL_LIBRARY        vnl )
find_library(VCL_LIBRARY        vcl )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )
find_library(JPEG_LIBRARY libjpeg)
find_path(VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)

fwInclude(${ZLIB_INCLUDE_DIRS})

fwForwardInclude(
    ${ITK_INCLUDE_DIRS}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
)
fwLink(
    ITKCommon
    ITKIOImageBase
    ITKIOJPEG
    ITKMetaIO
    itksys
    itkopenjpeg

    ${ZLIB_LIBRARIES}
    ${JPEG_LIBRARY}
    ${VGL_ALGO_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
)

