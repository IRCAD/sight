/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/patch/semantic/V09ALA/V10/data/Image.hpp"

#include <io/atoms/patch/SemanticPatchDB.hpp>

namespace sight::io::patch::semantic
{

namespace V09ALA
{

namespace V10
{

namespace data
{

/// Registers contextual patches dedicated to conversions from version 'V2' to version 'V03AGO'.
struct runner
{
    runner()
    {
        io::atoms::patch::SemanticPatchDB::sptr contextPatchDB = io::atoms::patch::SemanticPatchDB::getDefault();
        contextPatchDB->registerPatch(io::patch::semantic::V09ALA::V10::data::Image::New());
    }

    static runner r;
};

runner runner::r;

} // namespace data

} // namespace V10

} // namespace V09ALA

} // namespace sight::io::patch::semantic
