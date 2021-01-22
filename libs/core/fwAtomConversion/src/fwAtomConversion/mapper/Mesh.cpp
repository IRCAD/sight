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

#include "fwAtomConversion/mapper/Mesh.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/registry/macros.hpp"

#include <core/tools/UUID.hpp>

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>

#include <fwData/Mesh.hpp>

namespace fwAtomConversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( ::fwAtomConversion::mapper::Mesh, ::fwData::Mesh);

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr Mesh::convert( ::fwData::Object::sptr object,
                                       DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    ::fwAtomConversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    ::fwAtoms::Object::sptr atom = visitor.getAtomObject();

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(object);

    const auto meshAttributes = mesh->getAttributes();

    ::fwAtoms::Numeric::sptr attributes = ::fwAtoms::Numeric::New(static_cast<int>(meshAttributes));

    atom->setAttribute("attributes", attributes );

    return atom;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Mesh::convert(  ::fwAtoms::Object::sptr atom,
                                       AtomVisitor::DataCacheType& cache,
                                       const AtomVisitor::IReadPolicy& uuidPolicy
                                       )
{
    ::fwAtomConversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    ::fwData::Object::sptr data = visitor.getDataObject();
    ::fwData::Mesh::sptr mesh   = ::fwData::Mesh::dynamicCast(data);

    ::fwAtoms::Numeric::sptr attributes = ::fwAtoms::Numeric::dynamicCast( atom->getAttribute("attributes") );

    ::fwData::Mesh::Attributes meshAttributes = static_cast< ::fwData::Mesh::Attributes >(attributes->getValue<int>());

    mesh->setAttributes(meshAttributes);

    return mesh;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace fwAtomConversion
