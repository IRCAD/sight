#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/List.hpp"

fwCampImplementDataMacro((fwData)(List))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("values", &::fwData::List::m_attrContainer)
        ;
}

