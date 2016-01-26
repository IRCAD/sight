/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATIONACTIVITY_BUILDER_TOOLCALIBRATIONBUILDER_HPP__
#define __CALIBRATIONACTIVITY_BUILDER_TOOLCALIBRATIONBUILDER_HPP__

#include "calibrationActivity/config.hpp"
#include <fwActivities/IBuilder.hpp>
#include <fwActivities/builder/ActivitySeriesInitData.hpp>

namespace calibrationActivity
{
namespace builder
{
/**
 * @brief Defines an specific builder for the ToolCalibration activity.
 * @class ToolCalibrationBuilder
 */
class CALIBRATIONACTIVITY_CLASS_API ToolCalibrationBuilder : public ::fwActivities::builder::ActivitySeriesInitData
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolCalibrationBuilder)(::fwActivities::IBuilder),
                                            (()),
                                            ::fwActivities::builder::factory::New< ToolCalibrationBuilder > );

    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    CALIBRATIONACTIVITY_API ToolCalibrationBuilder(::fwActivities::IBuilder::Key key);

    /// Destructor. Do nothing.
    CALIBRATIONACTIVITY_API virtual ~ToolCalibrationBuilder();

    /**
     * @brief Ask user which tag(s) will be tracked.
     * @see ::fwActivities::builder::ActivitySeries::buildData
     */
    CALIBRATIONACTIVITY_API virtual ::fwMedData::ActivitySeries::sptr buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        SPTR(::fwData::Vector) currentSelection ) const;
};

} // namespace builder
} // namespace calibrationActivity

#endif /*__CALIBRATIONACTIVITY_BUILDER_TOOLCALIBRATIONBUILDER_HPP__*/
