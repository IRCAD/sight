/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/patch/structural/data/Acquisition/V1ToFwMedDataImageSeriesV1.hpp"
#include "io/patch/structural/data/ActivitySeries/V1ToV2.hpp"
#include "io/patch/structural/data/ActivitySeries/V2ToV1.hpp"
#include "io/patch/structural/data/AttachmentSeries/V1ToV2.hpp"
#include "io/patch/structural/data/AttachmentSeries/V2ToV1.hpp"
#include "io/patch/structural/data/Camera/V1ToV2.hpp"
#include "io/patch/structural/data/Camera/V2ToV1.hpp"
#include "io/patch/structural/data/Camera/V2ToV3.hpp"
#include "io/patch/structural/data/Camera/V3ToV2.hpp"
#include "io/patch/structural/data/CameraSeries/V2ToV3.hpp"
#include "io/patch/structural/data/CameraSeries/V3ToV2.hpp"
#include "io/patch/structural/data/DicomSeries/V1ToV2.hpp"
#include "io/patch/structural/data/DicomSeries/V2ToV1.hpp"
#include "io/patch/structural/data/Image/V1ToV2.hpp"
#include "io/patch/structural/data/Image/V2ToV3.hpp"
#include "io/patch/structural/data/Image/V3ToV2.hpp"
#include "io/patch/structural/data/ImageSeries/V1ToV2.hpp"
#include "io/patch/structural/data/ImageSeries/V2ToV1.hpp"
#include "io/patch/structural/data/ImageSeries/V2ToV3.hpp"
#include "io/patch/structural/data/ImageSeries/V3ToV2.hpp"
#include "io/patch/structural/data/Material/V1ToV2.hpp"
#include "io/patch/structural/data/Material/V2ToV3.hpp"
#include "io/patch/structural/data/Material/V3ToV4.hpp"
#include "io/patch/structural/data/Material/V4ToV3.hpp"
#include "io/patch/structural/data/Matrix4/V1ToV2.hpp"
#include "io/patch/structural/data/Mesh/V1ToV2.hpp"
#include "io/patch/structural/data/Mesh/V1ToV3.hpp"
#include "io/patch/structural/data/Mesh/V2ToV3.hpp"
#include "io/patch/structural/data/Mesh/V3ToV4.hpp"
#include "io/patch/structural/data/Mesh/V4ToV3.hpp"
#include "io/patch/structural/data/ModelSeries/V1ToV2.hpp"
#include "io/patch/structural/data/ModelSeries/V2ToV1.hpp"
#include "io/patch/structural/data/ModelSeries/V2ToV3.hpp"
#include "io/patch/structural/data/ModelSeries/V3ToV2.hpp"
#include "io/patch/structural/data/NavigationSeries/V1ToV2.hpp"
#include "io/patch/structural/data/NavigationSeries/V2ToV1.hpp"
#include "io/patch/structural/data/Patient/V1ToFwMedDataPatientV1.hpp"
#include "io/patch/structural/data/Reconstruction/V1ToV2.hpp"
#include "io/patch/structural/data/Reconstruction/V2ToV3.hpp"
#include "io/patch/structural/data/Study/V1ToFwMedDataStudyV1.hpp"
#include "io/patch/structural/data/Study/V1ToV2.hpp"
#include "io/patch/structural/data/Study/V2ToV1.hpp"

#include <io/atoms/patch/StructuralPatchDB.hpp>

namespace sight::io::patch::structural
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {
        io::atoms::patch::StructuralPatchDB::sptr structuralPatches = io::atoms::patch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(patch::structural::data::Reconstruction::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Reconstruction::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Patient::V1ToFwMedDataPatientV1::New());
        structuralPatches->registerPatch(patch::structural::data::Study::V1ToFwMedDataStudyV1::New());
        structuralPatches->registerPatch(patch::structural::data::Acquisition::V1ToFwMedDataImageSeriesV1::New());
        structuralPatches->registerPatch(patch::structural::data::Image::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Image::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Image::V3ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Material::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Material::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Material::V3ToV4::New());
        structuralPatches->registerPatch(patch::structural::data::Material::V4ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Mesh::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Mesh::V1ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Mesh::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Mesh::V3ToV4::New());
        structuralPatches->registerPatch(patch::structural::data::Mesh::V4ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Matrix4::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::ActivitySeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::ActivitySeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::AttachmentSeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::AttachmentSeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::DicomSeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::DicomSeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::ImageSeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::ImageSeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::ImageSeries::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::ImageSeries::V3ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::ModelSeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::ModelSeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::ModelSeries::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::ModelSeries::V3ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::NavigationSeries::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::NavigationSeries::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::Study::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Study::V2ToV1::New());

        structuralPatches->registerPatch(patch::structural::data::Camera::V1ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::Camera::V2ToV1::New());
        structuralPatches->registerPatch(patch::structural::data::Camera::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::Camera::V3ToV2::New());
        structuralPatches->registerPatch(patch::structural::data::CameraSeries::V2ToV3::New());
        structuralPatches->registerPatch(patch::structural::data::CameraSeries::V3ToV2::New());
    }

    static runner r;
};
runner runner::r;

} // namespace sight::io::patch::structural
