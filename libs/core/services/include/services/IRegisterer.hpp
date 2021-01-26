/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "services/config.hpp"
#include "services/IService.hpp"

namespace sight::services
{
/**
 * @brief   This interface defines registerer service API.
 *          Must be implemented for services that register objects together.
 *
 * @section Slots Slots
 * - \b computeRegistration(core::HiResClock::HiResClockType) : Compute the registration matrix.
 */
class SERVICES_CLASS_API IRegisterer : public services::IService
{
public:

    fwCoreServiceMacro(IRegisterer, services::IService)

    /**
     * @name Slots API
     * @{
     */
    SERVICES_API static const core::com::Slots::SlotKeyType s_COMPUTE_REGISTRATION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    SERVICES_API IRegisterer() noexcept;

    /**
     * @brief Destructor.
     */
    SERVICES_API virtual ~IRegisterer() noexcept;

protected:

    /// Register matrix slot
    SERVICES_API virtual void computeRegistration(core::HiResClock::HiResClockType timestamp) = 0;
};

} // namespace sight::services
