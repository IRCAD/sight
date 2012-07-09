#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/ResectionDB.hpp"

fwCampImplementDataMacro((fwData)(ResectionDB))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("safe_resection", &::fwData::ResectionDB::m_safeResection)
        .property("resections", &::fwData::ResectionDB::m_attrResections)
        ;
}
