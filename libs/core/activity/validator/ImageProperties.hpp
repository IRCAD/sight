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
#include "activity/IObjectValidator.hpp"

#include <core/base.hpp>

namespace sight::activity
{

namespace validator
{

/**
 * @brief Defines a validator which checks that properties of given images are the same.
 */
class ACTIVITY_CLASS_API ImageProperties : public activity::IObjectValidator
{
public:

    SIGHT_DECLARE_CLASS(
        ImageProperties,
        activity::IObjectValidator,
        activity::validator::factory::New<ImageProperties>
    )

    /// Constructor. Do nothing.
    ACTIVITY_API ImageProperties(activity::IValidator::Key key);

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~ImageProperties();

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @see activity::IValidator::validate
     */
    ACTIVITY_API virtual IValidator::ValidationType validate(
        const activity::extension::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection
    ) const override;

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @note Given object must be a Vector or Composite of ImageSeries (or images)
     * @see activity::IValidator::validate
     */
    ACTIVITY_API virtual IValidator::ValidationType validate(const CSPTR(data::Object)& currentObject) const
    override;
};

} // namespace validator

} // namespace sight::activity
