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

#include "io/atoms/patch/IStructuralPatch.hpp"

#include "io/atoms/patch/helper/functions.hpp"

#include <io/atoms/patch/types.hpp>

namespace sight::io::atoms::patch
{

IStructuralPatch::IStructuralPatch() :
    IPatch()
{
}

// ----------------------------------------------------------------------------

IStructuralPatch::~IStructuralPatch()
{
}

// ----------------------------------------------------------------------------

IStructuralPatch::IStructuralPatch( const IStructuralPatch& cpy ) :
    IPatch(cpy)
{
    m_originClassname = cpy.getOriginClassname();
    m_targetClassname = cpy.getTargetClassname();
    m_originVersion   = cpy.getOriginVersion();
    m_targetVersion   = cpy.getTargetVersion();
}

// ----------------------------------------------------------------------------

void IStructuralPatch::updateVersion(sight::atoms::Object::sptr current)
{
    io::atoms::patch::helper::setClassname(current, m_targetClassname);
    io::atoms::patch::helper::setVersion(current, m_targetVersion);
}

// ----------------------------------------------------------------------------

void IStructuralPatch::apply(const sight::atoms::Object::sptr& previous,
                             const sight::atoms::Object::sptr& current,
                             io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    SIGHT_ASSERT("The type of the previous object (" << io::atoms::patch::helper::getClassname(
                     previous)
                                                     << ") does not match the required type (" << m_originClassname << ").",
                 io::atoms::patch::helper::getClassname(previous) == m_originClassname);
    SIGHT_ASSERT("The version of the previous object (" << io::atoms::patch::helper::getVersion(
                     previous)
                                                        << ") does not match the required version (" << m_originVersion << ").",
                 io::atoms::patch::helper::getVersion(previous) == m_originVersion);
}

// ----------------------------------------------------------------------------

const std::string& IStructuralPatch::getTargetClassname() const
{
    return m_targetClassname;
}

// ----------------------------------------------------------------------------

const std::string& IStructuralPatch::getTargetVersion() const
{
    return m_targetVersion;
}

} //fwAtomsPatch
