/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "io/patch/config.hpp"
#include "io/patch/structural/creator/data/Series1.hpp"

namespace sight::io::patch::structural
{

namespace creator
{

namespace data
{

/// Structural creator for data::ModelSeries version '1'.
class IO_PATCH_CLASS_API ModelSeries1 : public io::patch::structural::creator::data::Series1
{
public:

    SIGHT_DECLARE_CLASS(ModelSeries1, io::patch::structural::creator::data::Series1, new ModelSeries1);

    /// Constructor
    IO_PATCH_API ModelSeries1();

    /// Destructor
    IO_PATCH_API ~ModelSeries1();

    /// Copy constructor
    IO_PATCH_API ModelSeries1(const ModelSeries1& cpy);

    /// Creates the specified object (sets 'reconstruction_db' attribute) and calls creator 'Series1'.
    IO_PATCH_API sight::atoms::Object::sptr create() override;
};

} // namespace data

} // namespace creator

} // namespace sight::io::patch::structural
