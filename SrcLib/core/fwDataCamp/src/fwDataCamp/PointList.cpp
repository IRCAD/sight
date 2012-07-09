#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/PointList.hpp"

fwCampImplementDataMacro((fwData)(PointList))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("points", &::fwData::PointList::m_vPoints)
        ;
}
