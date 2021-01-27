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

#include "activities/config.hpp"
#include "activities/IValidator.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

namespace sight::activities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that given series refer to the same study.
 */
class ACTIVITIES_CLASS_API RelatedStudy : public activities::IValidator
{
public:
    fwCoreClassMacro(RelatedStudy, activities::IValidator, activities::validator::factory::New< RelatedStudy >)

    /// Constructor. Do nothing.
    ACTIVITIES_API RelatedStudy(activities::IValidator::Key key);

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~RelatedStudy();

    /**
     * @see activities::IValidator::validate
     */
    ACTIVITIES_API virtual IValidator::ValidationType validate(
        const activities::registry::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection ) const override;
};

} // namespace validator
} // namespace sight::activities
