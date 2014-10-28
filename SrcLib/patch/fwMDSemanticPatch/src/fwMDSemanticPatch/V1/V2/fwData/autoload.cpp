/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

