#include "fwDataCamp/Factory.hpp"
#include <fwData/camp/mapper.hpp>
#include <fwCamp/Mapper/ArrayMapper.hpp>
#include "fwDataCamp/Graph.hpp"

fwCampImplementDataMacro((fwData)(Graph))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("nodes", &::fwData::Graph::m_nodes);
        //.property("connections", &::fwData::Graph::m_connections);
}
