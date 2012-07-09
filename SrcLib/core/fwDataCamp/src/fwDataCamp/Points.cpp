#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Point.hpp"

fwCampImplementDataMacro((fwData)(Point))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("coord", &::fwData::Point::m_vCoord);
}
