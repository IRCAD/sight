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

#include <io/atoms/patch/IStructuralCreator.hpp>

namespace sight::io::patch::structural
{

namespace creator
{

namespace data
{

/// Structural creator for data::Equipment version '1'.
class IO_PATCH_CLASS_API Equipment1 : public io::atoms::patch::IStructuralCreator
{
public:

    SIGHT_DECLARE_CLASS(Equipment1, io::atoms::patch::IStructuralCreator, new Equipment1);

    /// Constructor
    IO_PATCH_API Equipment1();

    /// Destructor
    IO_PATCH_API ~Equipment1();

    /// Copy constructor
    IO_PATCH_API Equipment1(const Equipment1& cpy);

    /// Create the specified object (sets 'institution_name' attribute).
    IO_PATCH_API sight::atoms::Object::sptr create() override;
};

} // namespace data

} // namespace creator

} // namespace sight::io::patch::structural
