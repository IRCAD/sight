/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

namespace fwMDSemanticPatch
{

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    versionManager->buildVersionTable(RC_PREFIX "/fwMDSemanticPatch_" FWMDSEMANTICPATCH_VER "/");
    versionManager->buildLinkTable(RC_PREFIX "/fwMDSemanticPatch_" FWMDSEMANTICPATCH_VER "/");
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return "V10";
}

} //namespace fwMDSemanticPatch

