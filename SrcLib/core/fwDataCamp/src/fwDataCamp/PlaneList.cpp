#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/PlaneList.hpp"

fwCampImplementDataMacro((fwData)(PlaneList))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("planes", &::fwData::PlaneList::m_vPlanes);
}
