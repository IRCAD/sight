/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwAtomConversion/mapper/Graph.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/registry/macros.hpp"

#include <fwAtoms/Map.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwData/Graph.hpp>

#include <fwTools/UUID.hpp>

namespace fwAtomConversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( ::fwAtomConversion::mapper::Graph, ::fwData::Graph);

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr Graph::convert( ::fwData::Object::sptr object,
                                        DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    ::fwAtomConversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    ::fwAtoms::Object::sptr atom = visitor.getAtomObject();

    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(object);

    ::fwAtoms::Sequence::sptr sequence = ::fwAtoms::Sequence::New();

    typedef ::fwData::Graph::ConnectionContainer GraphConnections;
    ::fwAtoms::Object::sptr value;
    for( GraphConnections::value_type elem :  graph->getConnections() )
    {
        value = ::fwAtoms::Object::New();

        value->setMetaInfo("ID_METAINFO", ::fwTools::UUID::generateUUID());

        value->setAttribute("edge", ::fwAtomConversion::convert(elem.first, cache));
        value->setAttribute("source", ::fwAtomConversion::convert(elem.second.first, cache));
        value->setAttribute("destination", ::fwAtomConversion::convert(elem.second.second, cache));

        sequence->push_back(value);
    }

    atom->setAttribute("connections", sequence );

    return atom;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Graph::convert(  ::fwAtoms::Object::sptr atom,
                                        AtomVisitor::DataCacheType& cache,
                                        const AtomVisitor::IReadPolicy& uuidPolicy
                                        )
{
    ::fwAtomConversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    ::fwData::Object::sptr data = visitor.getDataObject();
    ::fwData::Graph::sptr graph = ::fwData::Graph::dynamicCast(data);

    ::fwAtoms::Sequence::sptr seqAtom = ::fwAtoms::Sequence::dynamicCast( atom->getAttribute("connections") );
    for( ::fwAtoms::Base::sptr elemAtom  :  seqAtom->getValue() )
    {
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atoms stored in fwAtom::Sequence 'connections' must be atom objects"),
                               elemAtom->type() != ::fwAtoms::Base::OBJECT );

        ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast( elemAtom );
        ::fwAtoms::Object::sptr edgeAtom   = ::fwAtoms::Object::dynamicCast( objectAtom->getAttribute("edge") );
        ::fwData::Edge::sptr edge          =
            ::fwData::Edge::dynamicCast( ::fwAtomConversion::convert( edgeAtom, cache, uuidPolicy ) );

        ::fwAtoms::Object::sptr srcAtom = ::fwAtoms::Object::dynamicCast( objectAtom->getAttribute("source") );
        ::fwData::Node::sptr src        =
            ::fwData::Node::dynamicCast( ::fwAtomConversion::convert( srcAtom, cache, uuidPolicy ) );

        ::fwAtoms::Object::sptr destAtom = ::fwAtoms::Object::dynamicCast( objectAtom->getAttribute("destination") );
        ::fwData::Node::sptr dest        =
            ::fwData::Node::dynamicCast( ::fwAtomConversion::convert( destAtom, cache, uuidPolicy ) );

        graph->addEdge( edge, src, dest );
    }

    return graph;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace fwAtomConversion
