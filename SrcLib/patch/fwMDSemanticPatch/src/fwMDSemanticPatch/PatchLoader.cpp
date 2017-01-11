/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtomsPatch/VersionsManager.hpp>

#include "fwMDSemanticPatch/PatchLoader.hpp"

namespace fwMDSemanticPatch
{

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    versionManager->buildVersionTable("./share/fwMDSemanticPatch_" FWMDSEMANTICPATCH_VER "/");
    versionManager->buildLinkTable("./share/fwMDSemanticPatch_" FWMDSEMANTICPATCH_VER "/");
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return "V09ALA";
}

} //namespace fwMDSemanticPatch

