fwLoadProperties()

find_library(VNL_LIBRARY vnl)
find_library(VNL_ALGO_LIBRARY vnl_algo)
find_library(VCL_LIBRARY vcl )
find_package(CppUnit)


fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
)

fwLink(
    ${CPPUNIT_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
)
