/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

#include "fwAtomsPatch/SemanticPatchDB.hpp"
#include "fwAtomsPatch/ISemanticPatch.hpp"

namespace fwAtomsPatch
{


SemanticPatchDB::sptr SemanticPatchDB::s_default = ::boost::make_shared<SemanticPatchDB>();

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
