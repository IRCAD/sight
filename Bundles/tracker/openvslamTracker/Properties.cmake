
set( NAME openvslamTracker )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        fwCom
        fwData
        fwRuntime
        fwServices
        fwTools
        arServices
        fwVtkIO
        fwGui
        arData
        cvIO
        openvslamIO
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_OPENCV}
    ${CONAN_EIGEN}
    ${CONAN_OPENVSLAM})
