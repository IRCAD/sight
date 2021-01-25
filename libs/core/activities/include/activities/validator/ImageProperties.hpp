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
#include "activities/IObjectValidator.hpp"

#include <core/base.hpp>

namespace sight::activities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that properties of given images are the same.
 */
class ACTIVITIES_CLASS_API ImageProperties : public activities::IObjectValidator
{
public:
    fwCoreClassMacro(ImageProperties, activities::IObjectValidator,
                     activities::validator::factory::New< ImageProperties >)

    /// Constructor. Do nothing.
    ACTIVITIES_API ImageProperties(activities::IValidator::Key key);

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~ImageProperties();

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @see activities::IValidator::validate
     */
    ACTIVITIES_API virtual IValidator::ValidationType validate(
        const activities::registry::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection ) const override;

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @note Given object must be a Vector or Composite of ImageSeries (or images)
     * @see activities::IValidator::validate
     */
    ACTIVITIES_API virtual IValidator::ValidationType validate(const CSPTR(data::Object)& currentObject ) const
    override;
};

} // namespace validator
} // namespace sight::activities
