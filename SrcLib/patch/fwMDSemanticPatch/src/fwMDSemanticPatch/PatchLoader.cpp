/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwRuntime/operations.hpp>

namespace fwMDSemanticPatch
{

std::string PatchLoader::s_currentVersion = "V11";

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    auto path = ::fwRuntime::getLibraryResourceFilePath("fwMDSemanticPatch-" FWMDSEMANTICPATCH_VER "/");
    versionManager->buildVersionTable(path.string());
    versionManager->buildLinkTable(path.string());
}

//------------------------------------------------------------------------------

void PatchLoader::setCurrentVersion(const std::string& version)
{
    s_currentVersion = version;
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return s_currentVersion;
}

//------------------------------------------------------------------------------

} //namespace fwMDSemanticPatch
