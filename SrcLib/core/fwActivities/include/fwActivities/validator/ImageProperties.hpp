/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwActivities/config.hpp"
#include "fwActivities/IObjectValidator.hpp"

#include <fwCore/base.hpp>

namespace fwActivities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that properties of given images are the same.
 */
class FWACTIVITIES_CLASS_API ImageProperties : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageProperties)(::fwActivities::IObjectValidator),
                                            ::fwActivities::validator::factory::New< ImageProperties > )

    /// Constructor. Do nothing.
    FWACTIVITIES_API ImageProperties(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~ImageProperties();

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @see ::fwActivities::IValidator::validate
     */
    FWACTIVITIES_API virtual IValidator::ValidationType validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        const CSPTR(::fwData::Vector)& currentSelection ) const override;

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @note Given object must be a Vector or Composite of ImageSeries (or images)
     * @see ::fwActivities::IValidator::validate
     */
    FWACTIVITIES_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object)& currentObject ) const
    override;
};

} // namespace validator
} // namespace fwActivities
