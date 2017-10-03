/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

