fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwRuntime

    io
)


find_package(Boost REQUIRED)
find_package(ITK REQUIRED)

find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(VNL_LIBRARY        vnl )
find_library(VCL_LIBRARY        vcl )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )
find_library(NETLIB_LIBRARY     netlib )
find_path( VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)

fwForwardInclude(${Boost_INCLUDE_DIRS})
fwInclude(
    ${ITK_INCLUDE_DIRS}
    ${VNL_INCLUDE_PATH}
    )
fwLink(
    ITKCommon
    ITKDICOMParser
    ITKEXPAT
    ITKIO
    ITKMetaIO
    ITKNrrdIO
    ITKSpatialObject
    ITKniftiio
    ITKznz
    itkjpeg8
    itksys

    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    ${NETLIB_LIBRARY}
    )

