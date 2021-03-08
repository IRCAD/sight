/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "io/atoms/patch/IStructuralCreator.hpp"

#include "io/atoms/patch/helper/functions.hpp"

#include <atoms/Map.hpp>

#include <core/tools/UUID.hpp>

#include <io/atoms/patch/types.hpp>

namespace sight::io::atoms::patch
{

IStructuralCreator::IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::~IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::IStructuralCreator( const IStructuralCreator& cpy )
{
    m_classname = cpy.getObjectClassname();
    m_version   = cpy.getObjectVersion();
}

// ----------------------------------------------------------------------------

const std::string& IStructuralCreator::getObjectVersion() const
{
    return m_version;
}

// ----------------------------------------------------------------------------

const std::string& IStructuralCreator::getObjectClassname() const
{
    return m_classname;
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr IStructuralCreator::createObjBase() const
{
    sight::atoms::Object::sptr obj = sight::atoms::Object::New();

    io::atoms::patch::helper::setClassname(obj, m_classname);
    io::atoms::patch::helper::setVersion(obj, m_version);
    io::atoms::patch::helper::generateID( obj );

    // All data objects has a parameter fields
    io::atoms::patch::helper::cleanFields( obj );

    return obj;
}

// ----------------------------------------------------------------------------

} //fwAtomsPatch
