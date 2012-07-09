#include <fwCamp/Mapper/ValueMapper.hpp>
#include "fwDataCamp/Factory.hpp"
#include <fwData/camp/mapper.hpp>

#include "fwDataCamp/Composite.hpp"

fwCampImplementDataMacro((fwData)(Composite))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("values", &::fwData::Composite::m_attrContainer)
        ;
}
