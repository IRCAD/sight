fwLoadProperties()

fwUseForwardInclude(
    fwAtomConversion
    fwAtoms
    fwCore
    fwTools
)

find_package(CAMP REQUIRED)
find_package(Boost COMPONENTS log log_setup REQUIRED)

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

fwLink(
    ${CAMP_LIBRARIES}
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
)


