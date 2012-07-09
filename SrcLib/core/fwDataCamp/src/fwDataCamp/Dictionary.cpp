#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Dictionary.hpp"

fwCampImplementDataMacro((fwData)(Dictionary))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("dictionary_organs", &::fwData::Dictionary::m_attrDictionaryOrgans)
        ;
}

