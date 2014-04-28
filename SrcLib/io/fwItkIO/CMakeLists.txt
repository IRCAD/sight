fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwCore
    fwData
    fwDataIO
    fwTools
)



find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(ITK REQUIRED)
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})


find_library(VGL_ALGO_LIBRARY   vgl_algo )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(VNL_LIBRARY        vnl )
find_library(VCL_LIBRARY        vcl )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )
find_path( VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)


fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${ITK_INCLUDE_DIRS}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
    )
fwLink(
    ${Boost_LIBRARIES} 
    ITKCommon
    ITKIO
    ITKMetaIO
    ITKznz
    itksys
    ${ITKIO_LIB_DEPENDS}

    ${VGL_ALGO_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    gdcmMSFF
    )

