/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/patch/semantic/PatchLoader.hpp"

#include <core/runtime/operations.hpp>

#include <io/atoms/patch/VersionsManager.hpp>

namespace sight::io::patch::semantic
{

std::string PatchLoader::s_currentVersion = "V17AR";

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(io::atoms::patch::VersionsManager) versionManager = io::atoms::patch::VersionsManager::getDefault();
    auto path = core::runtime::getLibraryResourceFilePath("io_patch");
    versionManager->buildVersionTable(path.string());
    versionManager->buildLinkTable(path.string());

    io::patch::semantic::PatchLoader::setCurrentVersion(s_currentVersion);
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

} //namespace sight::io::patch::semantic
