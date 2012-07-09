#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/ROITraits.hpp"

fwCampImplementDataMacro((fwData)(ROITraits))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("name", &::fwData::ROITraits::m_identifier)
        .property("firstname", &::fwData::ROITraits::m_evaluatedExp)
        .property("node", &::fwData::ROITraits::m_structureTraits)
        .property("traits", &::fwData::ROITraits::m_maskOpNode)
        ;
}
