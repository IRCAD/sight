/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "service/config.hpp"

#include <service/IService.hpp>

namespace sight::service
{
/**
 * @brief   ICalibration is an interface API for calibration services.
 */
class SERVICE_CLASS_API ICalibration : public service::IService
{
public:
    fwCoreServiceMacro(ICalibration, service::IService)

    /// Constructor.
    SERVICE_API ICalibration() noexcept;

    /// Destructor.
    SERVICE_API virtual ~ICalibration() noexcept;
};

} // namespace sight::service
