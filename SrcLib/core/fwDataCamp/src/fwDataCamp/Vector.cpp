#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Vector.hpp"

fwCampImplementDataMacro((fwData)(Vector))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
           .property("values", &::fwData::Vector::m_attrContainer);
}
