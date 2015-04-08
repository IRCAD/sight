fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwTest
    fwMath
)

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})

fwLink(${CPPUNIT_LIBRARY})


find_library(VNL_LIBRARY vnl)
find_library(VNL_ALGO_LIBRARY vnl_algo)
find_library(VCL_LIBRARY vcl)

find_path(VNL_INCLUDE_PATH  vxl/core/vnl/vnl_alloc.h)

fwForwardInclude(
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
)

fwLink(
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
)

