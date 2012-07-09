#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Tag.hpp"

fwCampImplementDataMacro((fwData)(Tag))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("point_list", &::fwData::Tag::m_pointList)
        .property("type", &::fwData::Tag::m_sType)
        .property("size", &::fwData::Tag::m_size)
        ;
}
