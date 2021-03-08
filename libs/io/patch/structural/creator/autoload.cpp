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

#include "io/patch/structural/creator/data/ActivitySeries1.hpp"
#include "io/patch/structural/creator/data/Equipment1.hpp"
#include "io/patch/structural/creator/data/ImageSeries1.hpp"
#include "io/patch/structural/creator/data/Landmarks1.hpp"
#include "io/patch/structural/creator/data/ModelSeries1.hpp"
#include "io/patch/structural/creator/data/Patient1.hpp"
#include "io/patch/structural/creator/data/PointList1.hpp"
#include "io/patch/structural/creator/data/Study1.hpp"

#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{
namespace creator
{

/// Registers structural creators dedicated to data creation.
struct runner
{
    runner()
    {
        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();
        creators->registerCreator(io::patch::structural::creator::data::Equipment1::New());
        creators->registerCreator(io::patch::structural::creator::data::Patient1::New());
        creators->registerCreator(io::patch::structural::creator::data::Study1::New());
        creators->registerCreator(io::patch::structural::creator::data::ModelSeries1::New());
        creators->registerCreator(io::patch::structural::creator::data::ImageSeries1::New());
        creators->registerCreator(io::patch::structural::creator::data::ActivitySeries1::New());
        creators->registerCreator(io::patch::structural::creator::data::ActivitySeries1::New());
        creators->registerCreator(io::patch::structural::creator::data::Landmarks1::New());
        creators->registerCreator(io::patch::structural::creator::data::PointList1::New());
    }

    static runner r;
};

runner runner::r;

} // namespace creator
} // namespace sight::io::patch::structural
