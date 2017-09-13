/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwRuntime/operations.hpp>

namespace fwMDSemanticPatch
{

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    auto path = ::fwRuntime::getLibraryResourceFilePath("fwMDSemanticPatch_" FWMDSEMANTICPATCH_VER "/");
    versionManager->buildVersionTable(path.string());
    versionManager->buildLinkTable(path.string());
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return "V10";
}

//------------------------------------------------------------------------------

} //namespace fwMDSemanticPatch

