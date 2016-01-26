/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARMDSEMANTICPATCH_PATCHLOADER_HPP__
#define __ARMDSEMANTICPATCH_PATCHLOADER_HPP__

#include "arMDSemanticPatch/config.hpp"

namespace arMDSemanticPatch
{

/// Loads patches by processing .versions and .graphlink files.
class PatchLoader
{
public:
    /// Computes VersionsManager's link and version tables.
    ARMDSEMANTICPATCH_API static void loadPatches();
};

} //namespace arMDSemanticPatch

#endif /* __ARMDSEMANTICPATCH_PATCHLOADER_HPP__ */

