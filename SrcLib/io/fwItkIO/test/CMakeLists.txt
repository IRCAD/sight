fwLoadProperties()
find_package(CppUnit)
find_package(ITK REQUIRED)

find_library(VNL_LIBRARY vnl)
find_library(VNL_ALGO_LIBRARY vnl_algo)
find_library(V3P_NETLIB_LIBRARY v3p_netlib)
find_library(VCL_LIBRARY vcl)

fwInclude(${CPPUNIT_INCLUDE_DIR})

fwLink(
    ${CPPUNIT_LIBRARY}
    ${VNL_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    ${VCL_LIBRARY}
    
    ITKCommon
    itksys
)
