/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VALIDATORS_CAMERASERIES_STEREOCAMERA_HPP__
#define __VALIDATORS_CAMERASERIES_STEREOCAMERA_HPP__

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace CameraSeries
{
/**
 * @brief Defines a validator which checks if the current CameraSeries contains two calibrated cameras and an extrinsic
 *        matrix.
 */
class VALIDATORS_CLASS_API StereoCamera : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (StereoCamera)(::fwActivities::IObjectValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< StereoCamera > );

    /// Constructor. Do nothing.
    VALIDATORS_API StereoCamera(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~StereoCamera();

    /**
     * @brief Checks if the current CameraSeries contains two calibrated Cameras and an extrinsic matrix.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object)& currentObject ) const
    override;
};

} // namespace CameraSeries
} // namespace validators

#endif // __VALIDATORS_CAMERASERIES_STEREOCAMERA_HPP__

