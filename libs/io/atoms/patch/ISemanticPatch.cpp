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

#include "io/atoms/patch/ISemanticPatch.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/types.hpp"

#include <atoms/Object.hpp>

namespace sight::io::atoms::patch
{

ISemanticPatch::ISemanticPatch() :
    IPatch()
{
}

// ----------------------------------------------------------------------------

ISemanticPatch::~ISemanticPatch()
{
}

// ----------------------------------------------------------------------------

ISemanticPatch::ISemanticPatch(const ISemanticPatch& cpy) :
    IPatch(cpy)
{
    m_originClassname = cpy.getOriginClassname();
    m_originVersion   = cpy.getOriginVersion();
}

// ----------------------------------------------------------------------------

void ISemanticPatch::addContext(
    const std::string& context,
    const std::string& originVersion,
    const std::string& targetVersion
)
{
    core::mt::WriteLock lock(m_mutex);
    m_contexts.push_back(Context(context, originVersion, targetVersion));
}

// ----------------------------------------------------------------------------

bool ISemanticPatch::isApplicable(
    const std::string& context,
    const std::string& originVersion,
    const std::string& targetVersion
) const
{
    Context c(context, originVersion, targetVersion);
    core::mt::ReadLock lock(m_mutex);
    ContextVectorType::const_iterator it = std::find(m_contexts.begin(), m_contexts.end(), c);
    return it != m_contexts.end();
}

// ----------------------------------------------------------------------------

void ISemanticPatch::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    SIGHT_ASSERT(
        "The type of the previous object ("
        << io::atoms::patch::helper::getClassname(previous) << "does not match"
                                                               "the required type (" << m_originClassname << ").",
        io::atoms::patch::helper::getClassname(previous) == m_originClassname
    );

    SIGHT_ASSERT(
        "The version of the previous object (" << io::atoms::patch::helper::getVersion(previous)
        << "does not match the required version (" << m_originVersion << ").",
        io::atoms::patch::helper::getVersion(previous) == m_originVersion
    );
}

} //fwAtomsPatch
