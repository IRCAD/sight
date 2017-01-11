fwLoadProperties()

find_package(Qt5 COMPONENTS Core REQUIRED)

qt5_add_binary_resources(${FWPROJECT_NAME}_DARKSTYLE_RCC 
    "rc/darkstyle.qrc" 
    DESTINATION "${CMAKE_BINARY_DIR}/${BUNDLE_PREFIX}/${FWPROJECT_NAME}_${DASH_VERSION}/darkstyle.rcc"
)

add_dependencies(${FWPROJECT_NAME} ${FWPROJECT_NAME}_DARKSTYLE_RCC)

