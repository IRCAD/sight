/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMDSemanticPatch/config.hpp"

#include <string>

namespace fwMDSemanticPatch
{

/// Loads patches by processing .versions and .graphlink files.
class PatchLoader
{
public:
    /// Computes VersionsManager's link and version tables.
    FWMDSEMANTICPATCH_API static void loadPatches();

    /// Returns current MedicalData version.
    FWMDSEMANTICPATCH_API static std::string getCurrentVersion();

    /// Set the current MedicalData version.
    FWMDSEMANTICPATCH_API static void setCurrentVersion(const std::string& version);

private:

    /// Current version of the MedicalData context
    static std::string s_currentVersion;
};

} //namespace fwMDSemanticPatch
