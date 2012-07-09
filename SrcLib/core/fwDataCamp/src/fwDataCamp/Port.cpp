#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Port.hpp"

fwCampImplementDataMacro((fwData)(Port))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("identifier", &::fwData::Port::m_identifier)
        .property("type", &::fwData::Port::m_type)
        ;
}
