/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtomsPatch/SemanticPatchDB.hpp>

#include "fwMDSemanticPatch/V2/V03AGO/fwData/Image.hpp"

namespace fwMDSemanticPatch
{
namespace V2
{
namespace V03AGO
{
namespace fwData
{

/// Registers contextual patches dedicated to conversions from version 'V2' to version 'V03AGO'.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::SemanticPatchDB::sptr contextPatchDB = ::fwAtomsPatch::SemanticPatchDB::getDefault();
        contextPatchDB->registerPatch(::fwMDSemanticPatch::V2::V03AGO::fwData::Image::New());
    }

    static runner r;
};

runner runner::r;

} // namespace fwData
} // namespace V03AGO
} // namespace V2
} // namespace fwMDSemanticPatch

