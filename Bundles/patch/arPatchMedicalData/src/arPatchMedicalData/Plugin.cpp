/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
