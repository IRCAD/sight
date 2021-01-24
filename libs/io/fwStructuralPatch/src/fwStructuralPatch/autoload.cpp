/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwStructuralPatch/data/Acquisition/V1ToFwMedDataImageSeriesV1.hpp"
#include "fwStructuralPatch/data/ActivitySeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/ActivitySeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/AttachmentSeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/AttachmentSeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/DicomSeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/DicomSeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/Image/V1ToV2.hpp"
#include "fwStructuralPatch/data/Image/V2ToV3.hpp"
#include "fwStructuralPatch/data/Image/V3ToV2.hpp"
#include "fwStructuralPatch/data/ImageSeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/ImageSeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/ImageSeries/V2ToV3.hpp"
#include "fwStructuralPatch/data/ImageSeries/V3ToV2.hpp"
#include "fwStructuralPatch/data/Material/V1ToV2.hpp"
#include "fwStructuralPatch/data/Material/V2ToV3.hpp"
#include "fwStructuralPatch/data/Material/V3ToV4.hpp"
#include "fwStructuralPatch/data/Material/V4ToV3.hpp"
#include "fwStructuralPatch/data/Mesh/V1ToV2.hpp"
#include "fwStructuralPatch/data/Mesh/V1ToV3.hpp"
#include "fwStructuralPatch/data/Mesh/V2ToV3.hpp"
#include "fwStructuralPatch/data/Mesh/V3ToV4.hpp"
#include "fwStructuralPatch/data/Mesh/V4ToV3.hpp"
#include "fwStructuralPatch/data/ModelSeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/ModelSeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/ModelSeries/V2ToV3.hpp"
#include "fwStructuralPatch/data/ModelSeries/V3ToV2.hpp"
#include "fwStructuralPatch/data/NavigationSeries/V1ToV2.hpp"
#include "fwStructuralPatch/data/NavigationSeries/V2ToV1.hpp"
#include "fwStructuralPatch/data/Patient/V1ToFwMedDataPatientV1.hpp"
#include "fwStructuralPatch/data/Reconstruction/V1ToV2.hpp"
#include "fwStructuralPatch/data/Reconstruction/V2ToV3.hpp"
#include "fwStructuralPatch/data/Study/V1ToFwMedDataStudyV1.hpp"
#include "fwStructuralPatch/data/Study/V1ToV2.hpp"
#include "fwStructuralPatch/data/Study/V2ToV1.hpp"
#include "fwStructuralPatch/data/TransformationMatrix3D/V1ToV2.hpp"

#include <fwAtomsPatch/StructuralPatchDB.hpp>

namespace fwStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralPatchDB::sptr structuralPatches = ::fwAtomsPatch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(::fwStructuralPatchdata::Reconstruction::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Reconstruction::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Patient::V1ToFwMedDataPatientV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Study::V1ToFwMedDataStudyV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Acquisition::V1ToFwMedDataImageSeriesV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Image::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Image::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Image::V3ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Material::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Material::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Material::V3ToV4::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Material::V4ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Mesh::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Mesh::V1ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Mesh::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Mesh::V3ToV4::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Mesh::V4ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::TransformationMatrix3D::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ActivitySeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ActivitySeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::AttachmentSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::AttachmentSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::DicomSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::DicomSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ImageSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ImageSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ImageSeries::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ImageSeries::V3ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ModelSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ModelSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ModelSeries::V2ToV3::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::ModelSeries::V3ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::NavigationSeries::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::NavigationSeries::V2ToV1::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Study::V1ToV2::New());
        structuralPatches->registerPatch(::fwStructuralPatchdata::Study::V2ToV1::New());
    }

    static runner r;
};
runner runner::r;

} // namespace fwStructuralPatch
