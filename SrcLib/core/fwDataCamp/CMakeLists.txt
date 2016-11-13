add_definitions(-DCAMP_COMPILATION)

fwLoadProperties()


add_precompiled_header(${FWPROJECT_NAME} ../fwData/include/fwData/pch.hpp)
