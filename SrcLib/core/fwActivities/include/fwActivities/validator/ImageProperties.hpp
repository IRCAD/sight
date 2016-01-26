/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__
#define __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwActivities/IValidator.hpp"
#include "fwActivities/config.hpp"

namespace fwActivities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that properties of given images are the same.
 * @class ImageProperties.
 */
class FWACTIVITIES_CLASS_API ImageProperties : public ::fwActivities::IValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageProperties)(::fwActivities::IValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< ImageProperties > );


    /// Constructor. Do nothing.
    FWACTIVITIES_API ImageProperties(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~ImageProperties();

    /**
     * @see ::fwActivities::IValidator::validate
     */
    FWACTIVITIES_API virtual IValidator::ValidationType validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        SPTR(::fwData::Vector) currentSelection ) const;
};

} // namespace validator
} // namespace fwActivities

#endif // __FWACTIVITIES_VALIDATOR_IMAGEPROPERTIES_HPP__

