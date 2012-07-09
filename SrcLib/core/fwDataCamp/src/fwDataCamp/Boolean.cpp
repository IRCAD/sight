#include "fwDataCamp/Boolean.hpp"
#include "fwDataCamp/Factory.hpp"

fwCampImplementDataMacro((fwData)(Boolean))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("value",  (bool fwData::Boolean::*) &fwData::Boolean::m_value)
        ;
}
