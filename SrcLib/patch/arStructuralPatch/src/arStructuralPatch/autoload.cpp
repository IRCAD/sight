/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "arStructuralPatch/arData/Camera/V1ToV2.hpp"
#include "arStructuralPatch/arData/Camera/V2ToV1.hpp"
#include "arStructuralPatch/arData/Camera/V2ToV3.hpp"
#include "arStructuralPatch/arData/Camera/V3ToV2.hpp"

#include <fwAtomsPatch/StructuralPatchDB.hpp>

namespace arStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralPatchDB::sptr structuralPatches = ::fwAtomsPatch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(::arStructuralPatch::arData::Camera::V1ToV2::New());
        structuralPatches->registerPatch(::arStructuralPatch::arData::Camera::V2ToV1::New());
        structuralPatches->registerPatch(::arStructuralPatch::arData::Camera::V2ToV3::New());
        structuralPatches->registerPatch(::arStructuralPatch::arData::Camera::V3ToV2::New());
    }

    static runner r;
};
runner runner::r;

} // namespace arStructuralPatch
