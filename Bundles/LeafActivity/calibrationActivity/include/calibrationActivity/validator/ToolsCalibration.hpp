/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATIONACTIVITY_VALIDATOR_TOOLSCALIBRATION_HPP__
#define __CALIBRATIONACTIVITY_VALIDATOR_TOOLSCALIBRATION_HPP__

#include "calibrationActivity/config.hpp"

#include <fwActivities/IValidator.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>


namespace calibrationActivity
{
namespace validator
{

/**
 * @brief Defines a validator which checks that the selection contains 2 cameras series calibrated. One of the
 * cameraSeries contains 2 cameras, and the second contains one camera.
 * @class ToolsCalibration
 */
class CALIBRATIONACTIVITY_CLASS_API ToolsCalibration : public ::fwActivities::IValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolsCalibration)(::fwActivities::IValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< ToolsCalibration > );

    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    CALIBRATIONACTIVITY_API ToolsCalibration(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    CALIBRATIONACTIVITY_API virtual ~ToolsCalibration();

    /**
     * @see ::fwActivities::IValidator::validate
     */
    CALIBRATIONACTIVITY_API virtual IValidator::ValidationType validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        SPTR(::fwData::Vector) currentSelection ) const;
};

} // namespace validator
} // namespace calibrationActivity

#endif /*__CALIBRATIONACTIVITY_VALIDATOR_TOOLSCALIBRATION_HPP__*/


