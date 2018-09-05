/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arPatchMedicalData/Plugin.hpp"

#include <arStructuralPatch/PatchLoader.hpp>

#include <arMDSemanticPatch/PatchLoader.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace arPatchMedicalData
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::arPatchMedicalData::Plugin");

Plugin::~Plugin() noexcept
{
}

void Plugin::start()
{
    //Hack: force link with arStructuralPatch
    ::arStructuralPatch::PatchLoader::loadPatches();

    /* Force link with arMDSemanticPatch and compute version/link tables
     * for arMDSemanticPatch data.
     */
    ::arMDSemanticPatch::PatchLoader::loadPatches();
}

void Plugin::stop() noexcept
{
}

} // namespace arPatchMedicalData
