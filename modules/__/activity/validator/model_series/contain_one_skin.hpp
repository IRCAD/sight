/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include <activity/validator/object.hpp>

namespace sight::module::activity::validator::model_series
{

/**
 * @brief Defines a validator which checks that the ModelSeries contains one and only one Skin.
 * @note If the given Object is a container (Vector or Composite), it validates that each sub-object is a ModelSeries
 *       with one and only one Skin.
 */
class contain_one_skin : public sight::activity::validator::object
{
public:

    SIGHT_DECLARE_CLASS(
        contain_one_skin,
        sight::activity::validator::object,
        sight::activity::validator::factory::make<contain_one_skin>
    )
    /// Destructor. Do nothing.
    ~contain_one_skin() override;

    /**
     * @brief Checks that the ModelSeries contains one only one Skin.
     * @note Given object should be a single ModelSeries or a Vector or a Composite of ModelSeries.
     * @see module::activity::validator::base::validate
     */
    sight::activity::validator::return_t validate(const CSPTR(data::object)& _current_object) const
    override;
};

} // namespace sight::module::activity::validator::model_series
