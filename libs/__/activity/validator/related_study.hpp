/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/activity/config.hpp>

#include "activity/validator/base.hpp"

namespace sight::activity::validator
{

/**
 * @brief Defines a validator which checks that given series refer to the same study.
 */
class SIGHT_ACTIVITY_CLASS_API related_study : public activity::validator::base
{
public:

    SIGHT_DECLARE_CLASS(related_study, activity::validator::base);

    /// Destructor. Do nothing.
    SIGHT_ACTIVITY_API ~related_study() override = default;

    /**
     * @see activity::validator::base::validate
     */
    SIGHT_ACTIVITY_API validator::return_t validate(
        const activity::extension::activity_info& _activity_info,
        const CSPTR(data::vector)& _current_selection
    ) const override;
};

} // namespace sight::activity::validator
