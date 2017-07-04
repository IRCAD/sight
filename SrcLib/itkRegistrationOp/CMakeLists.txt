fwLoadProperties()

if(MSVC)
    target_compile_options(${NAME} PRIVATE "/bigobj")
endif()

find_package(ITK REQUIRED)

fwLink(
    ITKLabelMap
    ITKOptimizers
)
