/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "io/patch/config.hpp"

#include <string>

namespace sight::io::patch::semantic
{

/// Loads patches by processing .versions and .graphlink files.
class PatchLoader
{
public:

    /// Computes VersionsManager's link and version tables.
    IO_PATCH_API static void loadPatches();

    /// Returns current MedicalData version.
    IO_PATCH_API static std::string getCurrentVersion();

    /// Set the current MedicalData version.
    IO_PATCH_API static void setCurrentVersion(const std::string& version);

private:

    /// Current version of the MedicalData context
    static std::string s_currentVersion;
};

} //namespace sight::io::patch::semantic
