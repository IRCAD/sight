/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include <fwTools/UUID.hpp>

#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/types.hpp>

#include "fwAtomsPatch/IStructuralCreator.hpp"
#include "fwAtomsPatch/helper/functions.hpp"

namespace fwAtomsPatch
{

IStructuralCreator::IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::~IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::IStructuralCreator( const IStructuralCreator &cpy )
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

::fwAtoms::Object::sptr IStructuralCreator::createObjBase() const
{
    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::New();

    ::fwAtomsPatch::helper::setClassname(obj, m_classname);
    ::fwAtomsPatch::helper::setVersion(obj, m_version);
    ::fwAtomsPatch::helper::generateID( obj );

    // All data objects has a parameter fields
    fwAtomsPatch::helper::cleanFields( obj );

    return obj;
}

// ----------------------------------------------------------------------------

} //fwAtomsPatch
