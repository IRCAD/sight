fwLoadProperties()
find_package(Boost COMPONENTS system REQUIRED)
find_package(CppUnit)

find_library(VNL_LIBRARY vnl)
find_library(VNL_ALGO_LIBRARY vnl_algo)
find_library(VCL_LIBRARY vcl )


fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
)

fwLink(
    ${Boost_SYSTEM_LIBRARY}
    ${CPPUNIT_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
)
