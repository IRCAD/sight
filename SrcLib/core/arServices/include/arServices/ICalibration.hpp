/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "arServices/config.hpp"

#include <fwServices/IService.hpp>

namespace arServices
{
/**
 * @brief   ICalibration is an interface API for calibration services.
 */
class ARSERVICES_CLASS_API ICalibration : public fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro((ICalibration)(fwServices::IService))

    /// Constructor.
    ARSERVICES_API ICalibration() noexcept;

    /// Destructor.
    ARSERVICES_API virtual ~ICalibration() noexcept;
};

} // namespace arServices
