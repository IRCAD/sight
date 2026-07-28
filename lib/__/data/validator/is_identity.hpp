/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/validator/base.hpp>

namespace sight::data::validator
{

/**
 * @brief Defines a validator which checks if a matrix4 is the identity matrix.
 */
class is_identity final : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(is_identity, sight::data::validator::base, sight::data::validator::factory::make<is_identity>);
    ~is_identity() final = default;

    /**
     * @brief Checks that the object is a data::matrix4 and that it equals the identity matrix.
     * @see data::validator::base::validate
     */
    sight::data::validator::return_t validate(const sight::csptr<data::object>& _object) const final;
};

} // namespace sight::data::validator
