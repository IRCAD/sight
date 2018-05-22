/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwActivities/config.hpp"
#include "fwActivities/IValidator.hpp"

#include <fwCore/base.hpp>

#include <fwMedData/ActivitySeries.hpp>

namespace fwMedData
{
class ActivitySeries;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities activity validator's classes.
 *
 * This validator allows to check if all the required data are valid for the activity.
 */
class FWACTIVITIES_CLASS_API IActivityValidator : public ::fwActivities::IValidator
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IActivityValidator)(::fwActivities::IValidator) )

    /// Does nothing.
    FWACTIVITIES_API virtual ValidationType validate(
        const ::fwActivities::registry::ActivityInfo&,
        const CSPTR(::fwData::Vector)& ) const override
    {
        ValidationType validation;
        validation.first  = true;
        validation.second = "This validator does not validate this selection of data.";
        return validation;
    }

    /**
     * @brief Performs the validation of the given activity data.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains detailed error.
     */
    FWACTIVITIES_API virtual ValidationType validate(const CSPTR(::fwMedData::ActivitySeries)& activity ) const = 0;

    /**
     * @brief Checks if all the required data are present in the activity series.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains the list of missing
     *         (or not valid) data.
     */
    FWACTIVITIES_API ValidationType checkRequirements(const CSPTR(::fwMedData::ActivitySeries)& activity) const;

    /**
     * @brief Checks if all the activity's AppConfig parameters are valid.
     *
     * It checks if the object described with "camp" path are present in the activity.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains the list of missing
     *         (or not valid) data.
     */
    FWACTIVITIES_API ValidationType checkParameters(const CSPTR(::fwMedData::ActivitySeries)& activity) const;

    /**
     * @brief Calls the object validator if it is defined.
     * @param object object to validate
     * @param validatorImpl implementation of the validator to apply (if it is empty, check is assumed as valid).
     * @return pair <isValid, errorMsg> : errorMsg is empty if the object is valid else it contains the detailed error.
     * @see ::fwActivities::IObjectValidator
     */
    FWACTIVITIES_API ValidationType checkObject(const CSPTR(::fwData::Object)& object,
                                                const std::string& validatorImpl) const;
};

} // namespace fwActivities
