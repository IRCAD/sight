fwLoadProperties()
fwUseForwardInclude(
        fwComEd 
        fwCore 
        fwData 
        fwMath 
        fwRuntime 
        fwServices 
        fwTools 
        arData
        extData
)

find_package(OpenCV REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_imgproc
)

