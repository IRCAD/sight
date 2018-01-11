/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_ICALIBRATION_HPP__
#define __VIDEOCALIBRATION_ICALIBRATION_HPP__

#include "videoCalibration/config.hpp"

#include <fwServices/IService.hpp>

namespace videoCalibration
{
/**
 * @class   ICalibration
 * @brief   ICalibration is an interface API for calibration services.
 */
class VIDEOCALIBRATION_CLASS_API ICalibration : public fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro((ICalibration)(fwServices::IService));

    /// Constructor.
    VIDEOCALIBRATION_API ICalibration() noexcept;

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~ICalibration() noexcept;
};
} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_ICALIBRATION_HPP__
