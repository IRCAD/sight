/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwRuntime/operations.hpp>

namespace arMDSemanticPatch
{

std::string PatchLoader::s_currentVersion = "V13AR";

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    auto path = ::fwRuntime::getLibraryResourceFilePath("arMDSemanticPatch-" ARMDSEMANTICPATCH_VER "/");
    versionManager->buildVersionTable(path.string());
    versionManager->buildLinkTable(path.string());

    ::fwMDSemanticPatch::PatchLoader::setCurrentVersion(s_currentVersion);
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return s_currentVersion;
}

//------------------------------------------------------------------------------

} //namespace arMDSemanticPatch
