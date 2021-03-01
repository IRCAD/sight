/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "atoms/conversion/mapper/Graph.hpp"

#include "atoms/conversion/convert.hpp"
#include "atoms/conversion/exception/ConversionNotManaged.hpp"
#include "atoms/conversion/mapper/registry/macros.hpp"

#include <atoms/Map.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <data/Graph.hpp>

namespace sight::atoms::conversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( atoms::conversion::mapper::Graph, ::sight::data::Graph);

//-----------------------------------------------------------------------------

atoms::Object::sptr Graph::convert( data::Object::sptr object,
                                    DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    atoms::conversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    atoms::Object::sptr atom = visitor.getAtomObject();

    data::Graph::sptr graph = data::Graph::dynamicCast(object);

    atoms::Sequence::sptr sequence = atoms::Sequence::New();

    typedef data::Graph::ConnectionContainer GraphConnections;
    atoms::Object::sptr value;
    for( GraphConnections::value_type elem :  graph->getConnections() )
    {
        value = atoms::Object::New();

        value->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

        value->setAttribute("edge", atoms::conversion::convert(elem.first, cache));
        value->setAttribute("source", atoms::conversion::convert(elem.second.first, cache));
        value->setAttribute("destination", atoms::conversion::convert(elem.second.second, cache));

        sequence->push_back(value);
    }

    atom->setAttribute("connections", sequence );

    return atom;
}

//-----------------------------------------------------------------------------

data::Object::sptr Graph::convert(  atoms::Object::sptr atom,
                                    AtomVisitor::DataCacheType& cache,
                                    const AtomVisitor::IReadPolicy& uuidPolicy
                                    )
{
    atoms::conversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    data::Object::sptr data = visitor.getDataObject();
    data::Graph::sptr graph = data::Graph::dynamicCast(data);

    atoms::Sequence::sptr seqAtom = atoms::Sequence::dynamicCast( atom->getAttribute("connections") );
    for( atoms::Base::sptr elemAtom  :  seqAtom->getValue() )
    {
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atoms stored in fwAtom::Sequence 'connections' must be atom objects"),
                                  elemAtom->type() != atoms::Base::OBJECT );

        atoms::Object::sptr objectAtom = atoms::Object::dynamicCast( elemAtom );
        atoms::Object::sptr edgeAtom   = atoms::Object::dynamicCast( objectAtom->getAttribute("edge") );
        data::Edge::sptr edge          =
            data::Edge::dynamicCast( atoms::conversion::convert( edgeAtom, cache, uuidPolicy ) );

        atoms::Object::sptr srcAtom = atoms::Object::dynamicCast( objectAtom->getAttribute("source") );
        data::Node::sptr src        =
            data::Node::dynamicCast( atoms::conversion::convert( srcAtom, cache, uuidPolicy ) );

        atoms::Object::sptr destAtom = atoms::Object::dynamicCast( objectAtom->getAttribute("destination") );
        data::Node::sptr dest        =
            data::Node::dynamicCast( atoms::conversion::convert( destAtom, cache, uuidPolicy ) );

        graph->addEdge( edge, src, dest );
    }

    return graph;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace sight::atoms::conversion
