/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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
 * @brief Defines a validator which checks if a string_serializable data is true.
   - true : "true"/"TRUE"/"True"/"trUe"/... or any non-zero integers
   - false: all others values
 */
class is_true : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(
        is_true,
        sight::data::validator::base,
        sight::data::validator::factory::make<is_true>
    );
    ~is_true() override = default;

    /**
     * @brief Checks that the object has a true value
     * @note Given object should be a string_serializable data
     * @see data::validator::base::validate
     */
    sight::data::validator::return_t validate(const CSPTR(data::object)& _object) const override;

    /** Connects with data::MODIFIED signal **/
    SIGHT_DATA_API auto_connect_signals_t auto_connect_signals() const override;
};

} // namespace sight::data::validator
