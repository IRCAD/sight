
#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Edge.hpp"


fwCampImplementDataMacro((fwData)(Edge))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("nature", &::fwData::Edge::m_nature)
        .property("to_identifier", &::fwData::Edge::m_toPortIdentifier)
        .property("from_identifier", &::fwData::Edge::m_fromPortIdentifier)
        ;
}
