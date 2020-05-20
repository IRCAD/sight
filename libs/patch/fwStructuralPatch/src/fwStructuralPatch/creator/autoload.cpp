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

#include "fwStructuralPatch/creator/fwData/Landmarks1.hpp"
#include "fwStructuralPatch/creator/fwData/PointList1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ActivitySeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Equipment1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ImageSeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ModelSeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Patient1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Study1.hpp"

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{
namespace creator
{

/// Registers structural creators dedicated to data creation.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Equipment1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Patient1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Study1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ModelSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ImageSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwData::Landmarks1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwData::PointList1::New());
    }

    static runner r;
};

runner runner::r;

} // namespace creator
} // namespace fwStructuralPatch
