find_package (SOFA)
if(SOFA_FOUND)
    fwBundle(opSofa 0.1)

    fwUse(
        fwComEd
        fwCore
        fwData
        fwDataIO
        fwGui
        fwGuiQt
        fwMedData
        fwRenderVTK
        fwRuntime
        fwServices
        fwTools
        fwVtkIO

        visuVTKAdaptor
        gui
        io
    )
    fwUseForwardInclude(
        fwComEd
        fwData
        fwGui
        fwRenderVTK
        fwRuntime
        fwServices
        fwTools

        gui
        io
    )

    add_definitions('-DSOFA_HAVE_GLEW -DSOFA_HAVE_ZLIB -DSOFA_HAVE_PNG')

    find_package (Boost COMPONENTS filesystem REQUIRED)
    find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})

    find_package(OpenGL)

    fwForwardInclude(
        ${Boost_INCLUDE_DIRS} 
        ${QT_INCLUDE_DIR}
        ${SOFA_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        )
    fwLink(
        ${QT_LIBRARIES}
        ${Boost_LIBRARIES}
        ${SOFA_LIBRARIES}
        ${OPENGL_LIBRARIES}
        )

    set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
else(SOFA_FOUND)
    message(WARNING "Required package Sofa not found, bundle 'opSofa' disabled.")
endif(SOFA_FOUND)
