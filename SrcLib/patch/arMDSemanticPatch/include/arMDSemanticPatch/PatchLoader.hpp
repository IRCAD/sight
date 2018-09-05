/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "arMDSemanticPatch/config.hpp"

#include <string>

namespace arMDSemanticPatch
{

/// Loads patches by processing .versions and .graphlink files.
class PatchLoader
{
public:
    /// Computes VersionsManager's link and version tables.
    ARMDSEMANTICPATCH_API static void loadPatches();

    /// Returns current MedicalData version.
    ARMDSEMANTICPATCH_API static std::string getCurrentVersion();

private:

    static std::string s_currentVersion;
};

} //namespace arMDSemanticPatch
