/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwAtomsPatch/StructuralPatchDB.hpp>

#include "fwStructuralPatch/fwData/Reconstruction/V1ToV2.hpp"
#include "fwStructuralPatch/fwData/Reconstruction/V2ToV3.hpp"
#include "fwStructuralPatch/fwData/Patient/V1ToFwMedDataPatientV1.hpp"
#include "fwStructuralPatch/fwData/Study/V1ToFwMedDataStudyV1.hpp"
#include "fwStructuralPatch/fwData/Acquisition/V1ToFwMedDataImageSeriesV1.hpp"
#include "fwStructuralPatch/fwData/Mesh/V1ToV2.hpp"


namespace fwStructuralPatch
{

/// Registers structural patches dedicated to conversions from version '1' to version '2'.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralPatchDB::sptr structuralPatches = ::fwAtomsPatch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Reconstruction::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Reconstruction::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Patient::V1ToFwMedDataPatientV1::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Study::V1ToFwMedDataStudyV1::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Acquisition::V1ToFwMedDataImageSeriesV1::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Mesh::V1ToV2::New());
    }

    static runner r;
};
runner runner::r;

} // namespace fwStructuralPatch
