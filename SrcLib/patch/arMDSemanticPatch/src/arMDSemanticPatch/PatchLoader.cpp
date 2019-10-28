/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "arMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwRuntime/operations.hpp>

namespace arMDSemanticPatch
{

std::string PatchLoader::s_currentVersion = "V14AR";

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
