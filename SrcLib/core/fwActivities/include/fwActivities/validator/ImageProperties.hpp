/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__
#define __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__

#include "fwActivities/config.hpp"
#include "fwActivities/IObjectValidator.hpp"

#include <fwCore/base.hpp>

namespace fwActivities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that properties of given images are the same.
 * @class ImageProperties.
 */
class FWACTIVITIES_CLASS_API ImageProperties : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageProperties)(::fwActivities::IObjectValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< ImageProperties > );


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
        SPTR(::fwData::Vector) currentSelection ) const;

    /**
     * @brief Validates if the given images have the same properties (origin, spacing, ...)
     * @note Given object must be a Vector or Composite of ImageSeries (or images)
     * @see ::fwActivities::IValidator::validate
     */
    FWACTIVITIES_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object) &currentObject ) const;
};

} // namespace validator
} // namespace fwActivities

#endif // __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__

