fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataIO
    fwMemory
    fwTools
)

find_package(VTK REQUIRED)
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(
    ${GDCM_INCLUDE_DIRS}
)
fwLink(
    ${GDCM_LIBRARIES}
    
    vtkInteractionStyle
    vtkRenderingFreeTypeOpenGL
    vtkRenderingVolumeOpenGL
    vtkRenderingOpenGL
    vtkIOImage
    vtkCommonExecutionModel
    vtkCommonDataModel
    vtkCommonCore
    
    vtkgdcm
    gdcmMSFF
    gdcmDSED
    gdcmCommon
)

