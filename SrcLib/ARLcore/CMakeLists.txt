fwLoadProperties()


find_library( ANN_LIBRARY ANN )
find_path( ANN_INCLUDES  ANN/ANN.h)

find_library(VGL_ALGO_LIBRARY   vgl_algo )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
find_library(VNL_LIBRARY        vnl )
find_library(VGL_LIBRARY        vgl )
find_library(VCL_LIBRARY        vcl )
find_library(V3P_NETLIB_LIBRARY v3p_netlib )
find_library(NETLIB_LIBRARY     netlib )

find_path( VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)

fwForwardInclude(
    ${ANN_INCLUDES}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
    )

#add linked libraries
fwLink(
    ${ANN_LIBRARY}
    ${VGL_ALGO_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VGL_LIBRARY}
    ${VCL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    ${NETLIB_LIBRARY}
    )


