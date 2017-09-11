fwLoadProperties()

find_library(VNL_LIBRARY        vnl )
find_library(VNL_ALGO_LIBRARY   vnl_algo )
fwLink(
    ${VNL_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    )
