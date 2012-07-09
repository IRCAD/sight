#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/StructureTraitsDictionary.hpp"

fwCampImplementDataMacro((fwData)(StructureTraitsDictionary))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("structureTraits", &::fwData::StructureTraitsDictionary::m_structureTraitsMap)
        ;
}
