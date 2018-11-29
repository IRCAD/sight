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

#include <fwAtomsPatch/SemanticPatchDB.hpp>

#include "fwMDSemanticPatch/V1/V2/fwData/Composite.hpp"
#include "fwMDSemanticPatch/V1/V2/fwData/Study.hpp"
#include "fwMDSemanticPatch/V1/V2/fwData/Patient.hpp"
#include "fwMDSemanticPatch/V1/V2/fwData/Acquisition.hpp"

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

/// Registers contextual patches dedicated to conversions from version 'V1' to version 'V2'.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::SemanticPatchDB::sptr contextPatchDB = ::fwAtomsPatch::SemanticPatchDB::getDefault();
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V1::V2::fwData::Composite::New());
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V1::V2::fwData::Study::New());
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V1::V2::fwData::Patient::New());
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V1::V2::fwData::Acquisition::New());
    }

    static runner r;
};

runner runner::r;

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

