/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

namespace arMDSemanticPatch
{

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    versionManager->buildVersionTable("./share/arMDSemanticPatch-" ARMDSEMANTICPATCH_VER "/");
    versionManager->buildLinkTable("./share/arMDSemanticPatch-" ARMDSEMANTICPATCH_VER "/");
}

} //namespace arMDSemanticPatch

