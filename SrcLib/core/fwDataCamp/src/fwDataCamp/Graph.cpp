/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Graph.hpp>
#include <fwData/camp/mapper.hpp>
#include <fwCamp/Mapper/ArrayMapper.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Graph))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("nodes", &::fwData::Graph::m_nodes);
        //.property("connections", &::fwData::Graph::m_connections);
}
