#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/String.hpp"

fwCampImplementDataMacro((fwData)(String))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("value", (std::string fwData::String::*) &fwData::String::m_value)
        ;
}
