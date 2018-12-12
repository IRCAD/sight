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


#include "fwAtomsPatch/SemanticPatchDB.hpp"
#include "fwAtomsPatch/ISemanticPatch.hpp"

namespace fwAtomsPatch
{


SemanticPatchDB::sptr SemanticPatchDB::s_default = std::make_shared<SemanticPatchDB>();

// ----------------------------------------------------------------------------

SemanticPatchDB::SemanticPatchDB()
{
}

// ----------------------------------------------------------------------------

SemanticPatchDB::~SemanticPatchDB()
{
}

// ----------------------------------------------------------------------------

SemanticPatchDB::SemanticPatchDB( const SemanticPatchDB &cpy )
{
}

// ----------------------------------------------------------------------------

void SemanticPatchDB::registerPatch(::fwAtomsPatch::ISemanticPatch::sptr patch)
{
    VersionIDType origin = std::make_pair(patch->getOriginClassname(), patch->getOriginVersion());
    ::fwCore::mt::WriteLock lock(m_mutex);
    m_patches[origin].push_back(patch);
}

// ----------------------------------------------------------------------------

::fwAtomsPatch::ISemanticPatch::sptr SemanticPatchDB::getPatch(
    const std::string& context,
    const std::string& originVersion,
    const std::string& targetVersion,
    const std::string& objOriginClassname,
    const std::string& objOriginVersion) const
{
    VersionIDType origin = std::make_pair(objOriginClassname, objOriginVersion);

    ::fwCore::mt::ReadLock lock(m_mutex);
    PatchesType::const_iterator it = m_patches.find(origin);
    if(it != m_patches.end())
    {
        PatchVectorType::const_iterator itVec;
        for(itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
        {
            if((*itVec)->isApplicable(context, originVersion, targetVersion))
            {
                return *itVec;
            }
        }
    }

    return ::fwAtomsPatch::ISemanticPatch::sptr();
}

// ----------------------------------------------------------------------------

size_t SemanticPatchDB::size() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_patches.size();
}

// ----------------------------------------------------------------------------

SemanticPatchDB::sptr SemanticPatchDB::getDefault()
{
    return s_default;
}


} //fwAtomsPatch
