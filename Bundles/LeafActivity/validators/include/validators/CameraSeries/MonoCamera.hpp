/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VALIDATORS_CAMERASERIES_MONOCAMERA_HPP__
#define __VALIDATORS_CAMERASERIES_MONOCAMERA_HPP__

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace CameraSeries
{
/**
 * @brief Defines a validator which checks if the current CameraSeries contains one and only one Camera and is
 *        calibrated.
 */
class VALIDATORS_CLASS_API MonoCamera : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MonoCamera)(::fwActivities::IObjectValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< MonoCamera > );


    /// Constructor. Do nothing.
    VALIDATORS_API MonoCamera(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~MonoCamera();

    /**
     * @brief Checks if the current CameraSeries contains one and only one Camera and is calibrated.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object) &currentObject ) const;
};

} // namespace CameraSeries
} // namespace validators



#endif /* __VALIDATORS_CAMERASERIES_MONOCAMERA_HPP__ */
