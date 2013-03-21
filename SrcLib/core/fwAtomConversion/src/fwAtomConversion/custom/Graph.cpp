/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwData/Graph.hpp>

#include "fwAtomConversion/custom/Graph.hpp"

namespace fwAtomConversion
{
namespace custom
{

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr Graph::toMeta(::fwData::Object::sptr object,
                                    ::fwAtomConversion::AtomHelper& metaHelper)
{
    typedef ::fwData::Graph::ConnectionContainer GraphConnections;
    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(object);
    ::fwAtoms::Object::sptr meta = this->computeMinimalInfos(graph,metaHelper);

    ::fwAtoms::Map::sptr metaMap = ::fwAtoms::Map::New();


    const GraphConnections& connections = graph->getCRefConnections();
    GraphConnections::const_iterator cIt = connections.begin();


    ::fwAtoms::Object::sptr key;
    ::fwAtoms::Object::sptr value;
    for(;cIt != connections.end(); ++cIt)
    {
        key = metaHelper.dataToMeta(cIt->first);
        value = ::fwAtoms::Object::New();

        value->addAttribut("source", metaHelper.dataToMeta(cIt->second.first));
        value->addAttribut("destination", metaHelper.dataToMeta(cIt->second.second));

        metaMap->insert(key, value );
    }

    meta->addAttribut("connections", metaMap );
    return meta;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Graph::fromMeta(::fwAtoms::Object::sptr meta,
                                     ::fwAtomConversion::AtomHelper& metaHelper)
{
    ::fwData::Object::sptr object= this->computeMinimalInfos(meta,metaHelper);
    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(object);

    ::fwAtoms::Base::sptr metaAttr = meta->getAttribut("connections");
    ::fwAtoms::Map::sptr connections = ::fwAtoms::Map::dynamicCast(metaAttr);
    ::fwAtoms::Map::ConstIteratorType cIt = connections->begin();


    ::fwData::Object::sptr tmp;
    ::fwData::Edge::sptr key;
    ::fwData::Node::sptr source;
    ::fwData::Node::sptr target;

    ::fwAtoms::Object::sptr value;

    for(; cIt != connections->end(); ++cIt)
    {
        tmp = metaHelper.metaToData(::fwAtoms::Object::dynamicCast(cIt->first));
        key = ::fwData::Edge::dynamicCast(tmp);

        value = ::fwAtoms::Object::dynamicCast(cIt->second);

        tmp = metaHelper.metaToData(::fwAtoms::Object::dynamicCast(value->getAttribut("source")));
        source = ::fwData::Node::dynamicCast(tmp);

        tmp = metaHelper.metaToData(::fwAtoms::Object::dynamicCast(value->getAttribut("destination")));
        target = ::fwData::Node::dynamicCast(tmp);

        graph->addEdge(key, source ,target);
    }
    return graph;
}

//-----------------------------------------------------------------------------

} //namespace custom
} //namespace fwAtomConversion
