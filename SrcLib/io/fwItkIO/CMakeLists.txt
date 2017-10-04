fwLoadProperties()

find_package(ITK REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED )

find_library(JPEG_LIBRARY libjpeg)

find_library(VCL_LIBRARY        vcl )
find_library(VNL_LIBRARY        vnl )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )

fwInclude(${ZLIB_INCLUDE_DIRS})

fwForwardInclude(
    ${ITK_INCLUDE_DIRS}
)

fwLink(
    ${ZLIB_LIBRARIES}
    ${JPEG_LIBRARY}
)

# Since ITK is template-based, this make sense to force the link for dependencies
fwForwardLink(
    ITKCommon
    ITKIOImageBase
    ITKIOJPEG
    ITKMetaIO
    ${VCL_LIBRARY}
    ${VNL_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    itksys
    itkopenjpeg
)

