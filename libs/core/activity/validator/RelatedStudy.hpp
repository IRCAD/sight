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

#include "activity/config.hpp"
#include "activity/IValidator.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

namespace sight::activity
{
namespace validator
{

/**
 * @brief Defines a validator which checks that given series refer to the same study.
 */
class ACTIVITY_CLASS_API RelatedStudy : public activity::IValidator
{
public:
    SIGHT_DECLARE_CLASS(RelatedStudy, activity::IValidator, activity::validator::factory::New< RelatedStudy >)

    /// Constructor. Do nothing.
    ACTIVITY_API RelatedStudy(activity::IValidator::Key key);

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~RelatedStudy();

    /**
     * @see activity::IValidator::validate
     */
    ACTIVITY_API virtual IValidator::ValidationType validate(
        const activity::extension::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection ) const override;
};

} // namespace validator
} // namespace sight::activity
