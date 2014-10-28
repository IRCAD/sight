#cmake version
cmake_minimum_required(VERSION 2.8)

project(ARLcore)

set(TARGET_NAME ARLcore)

file( GLOB_RECURSE SOURCES src/* )
file( GLOB_RECURSE INCLUDES include/* )

set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}" )

find_package( Boost 1.40 REQUIRED )

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

include_directories(
    "include"
    ${Boost_INCLUDE_DIR}

    ${ANN_INCLUDES}

    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
    )

#declaration of target
add_library( ${TARGET_NAME} SHARED ${SOURCES} ${INCLUDES})

#add linked libraries
target_link_libraries(${TARGET_NAME}
    ${ANN_LIBRARY}
    ${VGL_ALGO_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VGL_LIBRARY}
    ${VCL_LIBRARY}
    ${V3P_NETLIB_LIBRARY}
    ${NETLIB_LIBRARY}
    )
set_target_properties(
    ${TARGET_NAME} PROPERTIES DEFINE_SYMBOL  "ARLCORE_EXPORTS" 
    )
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ARLcore)
set_target_properties(${TARGET_NAME} PROPERTIES VERSION 0.2)

