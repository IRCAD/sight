#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Float.hpp"

fwCampImplementDataMacro((fwData)(Float))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("value", (float fwData::Float::*) &fwData::Float::m_value)
        ;
}
