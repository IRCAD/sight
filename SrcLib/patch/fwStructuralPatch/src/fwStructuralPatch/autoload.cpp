/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwStructuralPatch/fwData/Acquisition/V1ToFwMedDataImageSeriesV1.hpp"
#include "fwStructuralPatch/fwData/Image/V1ToV2.hpp"
#include "fwStructuralPatch/fwData/Material/V1ToV2.hpp"
#include "fwStructuralPatch/fwData/Material/V2ToV3.hpp"
#include "fwStructuralPatch/fwData/Material/V3ToV4.hpp"
#include "fwStructuralPatch/fwData/Material/V4ToV3.hpp"
#include "fwStructuralPatch/fwData/Mesh/V1ToV2.hpp"
#include "fwStructuralPatch/fwData/Mesh/V1ToV3.hpp"
#include "fwStructuralPatch/fwData/Mesh/V2ToV3.hpp"
#include "fwStructuralPatch/fwData/Patient/V1ToFwMedDataPatientV1.hpp"
#include "fwStructuralPatch/fwData/Reconstruction/V1ToV2.hpp"
#include "fwStructuralPatch/fwData/Reconstruction/V2ToV3.hpp"
#include "fwStructuralPatch/fwData/Study/V1ToFwMedDataStudyV1.hpp"
#include "fwStructuralPatch/fwData/TransformationMatrix3D/V1ToV2.hpp"
#include "fwStructuralPatch/fwMedData/ImageSeries/V1ToV2.hpp"
#include "fwStructuralPatch/fwMedData/ImageSeries/V2ToV1.hpp"
#include "fwStructuralPatch/fwMedData/ModelSeries/V1ToV2.hpp"
#include "fwStructuralPatch/fwMedData/ModelSeries/V2ToV1.hpp"

#include <fwAtomsPatch/StructuralPatchDB.hpp>

namespace fwStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
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
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Image::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Material::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Material::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Material::V3ToV4::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Material::V4ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Mesh::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Mesh::V1ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::Mesh::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwData::TransformationMatrix3D::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwMedData::ImageSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwMedData::ImageSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwMedData::ModelSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatch::fwMedData::ModelSeries::V2ToV1::New());
    }

    static runner r;
};
runner runner::r;

} // namespace fwStructuralPatch
