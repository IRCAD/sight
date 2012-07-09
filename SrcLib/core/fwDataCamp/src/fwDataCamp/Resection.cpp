#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Resection.hpp"

fwCampImplementDataMacro((fwData)(Resection))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("name", &::fwData::Resection::m_name)
        .property("plane_list", &::fwData::Resection::m_planeList)
        .property("inputs", &::fwData::Resection::m_vInputs)
        .property("outputs", &::fwData::Resection::m_vOutputs)
        .property("is_safe_part", &::fwData::Resection::m_isSafePart)
        .property("is_valid", &::fwData::Resection::m_isValid)
        .property("is_visible", &::fwData::Resection::m_isVisible)
        ;
}
