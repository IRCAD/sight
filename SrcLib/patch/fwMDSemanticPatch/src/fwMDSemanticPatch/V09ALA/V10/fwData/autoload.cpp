/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/V09ALA/V10/fwData/Image.hpp"

#include <fwAtomsPatch/SemanticPatchDB.hpp>

namespace fwMDSemanticPatch
{
namespace V09ALA
{
namespace V10
{
namespace fwData
{

/// Registers contextual patches dedicated to conversions from version 'V2' to version 'V03AGO'.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::SemanticPatchDB::sptr contextPatchDB = ::fwAtomsPatch::SemanticPatchDB::getDefault();
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V09ALA::V10::fwData::Image::New());
    }

    static runner r;
};

runner runner::r;

} // namespace fwData
} // namespace V10
} // namespace V09ALA
} // namespace fwMDSemanticPatch

