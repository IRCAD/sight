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
#include "fwActivities/IValidator.hpp"

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwActivities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that given series refer to the same study.
 */
class FWACTIVITIES_CLASS_API RelatedStudy : public ::fwActivities::IValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (RelatedStudy)(::fwActivities::IValidator),
                                            ::fwActivities::validator::factory::New< RelatedStudy > );

    /// Constructor. Do nothing.
    FWACTIVITIES_API RelatedStudy(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~RelatedStudy();

    /**
     * @see ::fwActivities::IValidator::validate
     */
    FWACTIVITIES_API virtual IValidator::ValidationType validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        const CSPTR(::fwData::Vector)& currentSelection ) const override;
};

} // namespace validator
} // namespace fwActivities
