/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMetaData/String.hpp>
#include <fwMetaData/Map.hpp>
#include <fwMetaData/Sequence.hpp>
#include <fwData/Graph.hpp>

#include "fwMetaConversion/custom/Graph.hpp"

namespace fwMetaConversion
{
namespace custom
{

//-----------------------------------------------------------------------------

::fwMetaData::Object::sptr Graph::toMeta(::fwData::Object::sptr object,
                                    ::fwMetaConversion::MetaHelper& metaHelper)
{
    typedef ::fwData::Graph::ConnectionContainer GraphConnections;
    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(object);
    ::fwMetaData::Object::sptr meta = this->computeMinimalInfos(graph,metaHelper);

    ::fwMetaData::Map::sptr metaMap = ::fwMetaData::Map::New();


    const GraphConnections& connections = graph->getCRefConnections();
    GraphConnections::const_iterator cIt = connections.begin();


    ::fwMetaData::Object::sptr key;
    ::fwMetaData::Object::sptr value;
    for(;cIt != connections.end(); ++cIt)
    {
        key = metaHelper.dataToMeta(cIt->first);
        value = ::fwMetaData::Object::New();

        value->addAttribut("source", metaHelper.dataToMeta(cIt->second.first));
        value->addAttribut("destination", metaHelper.dataToMeta(cIt->second.second));

        metaMap->insert(key, value );
    }

    meta->addAttribut("connections", metaMap );
    return meta;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Graph::fromMeta(::fwMetaData::Object::sptr meta,
                                     ::fwMetaConversion::MetaHelper& metaHelper)
{
    ::fwData::Object::sptr object= this->computeMinimalInfos(meta,metaHelper);
    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(object);

    ::fwMetaData::Base::sptr metaAttr = meta->getAttribut("connections");
    ::fwMetaData::Map::sptr connections = ::fwMetaData::Map::dynamicCast(metaAttr);
    ::fwMetaData::Map::cIterator cIt = connections->cBegin();


    ::fwData::Object::sptr tmp;
    ::fwData::Edge::sptr key;
    ::fwData::Node::sptr source;
    ::fwData::Node::sptr target;

    ::fwMetaData::Object::sptr value;

    for(; cIt != connections->cEnd(); ++cIt)
    {
        tmp = metaHelper.metaToData(::fwMetaData::Object::dynamicCast(cIt->first));
        key = ::fwData::Edge::dynamicCast(tmp);

        value = ::fwMetaData::Object::dynamicCast(cIt->second);

        tmp = metaHelper.metaToData(::fwMetaData::Object::dynamicCast(value->getAttribut("source")));
        source = ::fwData::Node::dynamicCast(tmp);

        tmp = metaHelper.metaToData(::fwMetaData::Object::dynamicCast(value->getAttribut("destination")));
        target = ::fwData::Node::dynamicCast(tmp);

        graph->addEdge(key, source ,target);
    }
    return graph;
}

//-----------------------------------------------------------------------------

} //namespace custom
} //namespace fwMetaConversion
