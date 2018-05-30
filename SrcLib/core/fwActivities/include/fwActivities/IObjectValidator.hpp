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

namespace fwData
{
class Object;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities data validator's classes.
 *
 * This validator works on one type of data. The validated data can be a single data, or a Vector or a Composite
 * containing multiple data of the same type.
 */
class FWACTIVITIES_CLASS_API IObjectValidator : public ::fwActivities::IValidator
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IObjectValidator)(::fwActivities::IValidator) )

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
     * @brief Performs the validation of the given data.
     * @note  This data could be a single data, or a Vector or a Composite of one type of data.
     */
    FWACTIVITIES_API virtual ValidationType validate(const CSPTR(::fwData::Object)& currentData ) const = 0;

};

} // namespace fwActivities
