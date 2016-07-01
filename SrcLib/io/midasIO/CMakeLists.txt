fwLoadProperties()

find_package(LibXml2)
fwInclude(${LIBXML2_INCLUDE_DIR})
fwLink(${LIBXML2_LIBRARIES})

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(CryptoPP REQUIRED)
fwInclude(${CryptoPP_INCLUDE_DIR})
fwLink(${CryptoPP_LIBRARIES})

find_package(Qt5 COMPONENTS Core Gui Network REQUIRED)
fwForwardInclude(
    ${Qt5Core_INCLUDE_DIR}
    ${Qt5Gui_INCLUDE_DIR}
    ${Qt5Network_INCLUDE_DIR}
)
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Network_LIBRARIES}
)

find_package(VTK COMPONENTS vtkRenderingCore vtkIOExport NO_MODULE REQUIRED)
fwForwardInclude(${VTK_INCLUDE_DIRS})
fwForwardLink( ${VTK_LIBRARIES} )

fwForwardInclude(${EXTERNAL_LIBRARIES}/include)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
