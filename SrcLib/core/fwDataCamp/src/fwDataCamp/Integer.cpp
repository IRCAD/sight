#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Integer.hpp"

fwCampImplementDataMacro((fwData)(Integer))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("value", (int fwData::Integer::*) &fwData::Integer::m_value)
        ;
}
