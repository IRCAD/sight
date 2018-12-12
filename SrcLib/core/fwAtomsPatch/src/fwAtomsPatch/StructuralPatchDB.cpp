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


#include "fwAtomsPatch/StructuralPatchDB.hpp"

namespace fwAtomsPatch
{

StructuralPatchDB::sptr StructuralPatchDB::s_default = std::make_shared<StructuralPatchDB>();

void StructuralPatchDB::registerPatch(::fwAtomsPatch::IStructuralPatch::sptr patch)
{
    VersionIDType source = std::make_pair(patch->getOriginClassname(), patch->getOriginVersion());
    VersionIDType target = std::make_pair(patch->getTargetClassname(), patch->getTargetVersion());
    PatchKeyType key     = std::make_pair(source, target);
    ::fwCore::mt::WriteLock lock(m_mutex);
    m_patches[key] = patch;
}

// ----------------------------------------------------------------------------

::fwAtomsPatch::IStructuralPatch::sptr StructuralPatchDB::getPatch(const std::string& originClassname,
                                                                   const std::string& originVersion,
                                                                   const std::string& targetClassname,
                                                                   const std::string& targetVersion) const
{
    const VersionIDType source = std::make_pair(originClassname, originVersion);
    const VersionIDType target = std::make_pair(targetClassname, targetVersion);
    const PatchKeyType key     = std::make_pair(source, target);

    ::fwCore::mt::ReadLock lock(m_mutex);
    PatchesType::const_iterator it = m_patches.find(key);
    if(it != m_patches.end())
    {
        return it->second;
    }
    else
    {
        return ::fwAtomsPatch::IStructuralPatch::sptr();
    }
}

// ----------------------------------------------------------------------------

size_t StructuralPatchDB::size() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_patches.size();
}

} //fwAtomsPatch
