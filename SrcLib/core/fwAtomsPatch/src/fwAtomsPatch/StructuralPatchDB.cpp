/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

#include "fwAtomsPatch/StructuralPatchDB.hpp"

namespace fwAtomsPatch
{

StructuralPatchDB::sptr StructuralPatchDB::s_default = ::boost::make_shared<StructuralPatchDB>();

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
