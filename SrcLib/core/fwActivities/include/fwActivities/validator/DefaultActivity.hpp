/************************************************************************
 *
 * Copyright (C) 2016-2017 IRCAD France
 * Copyright (C) 2016-2017 IHU Strasbourg
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

#ifndef __FWACTIVITIES_VALIDATOR_DEFAULTACTIVITY_HPP__
#define __FWACTIVITIES_VALIDATOR_DEFAULTACTIVITY_HPP__

#include "fwActivities/config.hpp"
#include "fwActivities/IActivityValidator.hpp"

#include <fwCore/base.hpp>

namespace fwActivities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that activity requirements are valid.
 *
 * It checks if all the required data are present and if the appConfig parameters are found.
 */
class FWACTIVITIES_CLASS_API DefaultActivity : public ::fwActivities::IActivityValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (DefaultActivity)(::fwActivities::IActivityValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< DefaultActivity > )

    /// Constructor. Do nothing.
    FWACTIVITIES_API DefaultActivity(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~DefaultActivity();

    /// Checks if all the required data are present and if the appConfig parameters are found.
    FWACTIVITIES_API virtual IValidator::ValidationType validate(
        const CSPTR(::fwMedData::ActivitySeries)& activitySeries ) const override;
};

} // namespace validator
} // namespace fwActivities

#endif // __FWACTIVITIES_VALIDATOR_DEFAULTACTIVITY_HPP__

