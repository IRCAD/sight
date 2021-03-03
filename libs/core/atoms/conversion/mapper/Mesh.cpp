/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "atoms/conversion/mapper/Mesh.hpp"

#include "atoms/conversion/convert.hpp"
#include "atoms/conversion/exception/ConversionNotManaged.hpp"
#include "atoms/conversion/mapper/registry/macros.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>

#include <core/tools/UUID.hpp>

#include <data/Mesh.hpp>

namespace sight::atoms::conversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( atoms::conversion::mapper::Mesh, sight::data::Mesh);

//-----------------------------------------------------------------------------

atoms::Object::sptr Mesh::convert( data::Object::sptr object,
                                   DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    atoms::conversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    atoms::Object::sptr atom = visitor.getAtomObject();

    data::Mesh::sptr mesh = data::Mesh::dynamicCast(object);

    const auto meshAttributes = mesh->getAttributes();

    atoms::Numeric::sptr attributes = atoms::Numeric::New(static_cast<int>(meshAttributes));

    atom->setAttribute("attributes", attributes );

    return atom;
}

//-----------------------------------------------------------------------------

data::Object::sptr Mesh::convert(  atoms::Object::sptr atom,
                                   AtomVisitor::DataCacheType& cache,
                                   const AtomVisitor::IReadPolicy& uuidPolicy
                                   )
{
    atoms::conversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    data::Object::sptr data = visitor.getDataObject();
    data::Mesh::sptr mesh   = data::Mesh::dynamicCast(data);

    atoms::Numeric::sptr attributes = atoms::Numeric::dynamicCast( atom->getAttribute("attributes") );

    data::Mesh::Attributes meshAttributes = static_cast< data::Mesh::Attributes >(attributes->getValue<int>());

    mesh->setAttributes(meshAttributes);

    return mesh;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace sight::atoms::conversion
