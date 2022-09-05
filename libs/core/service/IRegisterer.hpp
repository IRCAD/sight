/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "service/config.hpp"
#include "service/IService.hpp"

namespace sight::service
{

/**
 * @brief   This interface defines registerer service API.
 *          Must be implemented for services that register objects together.
 *
 * @section Slots Slots
 * - \b computeRegistration(core::HiResClock::HiResClockType) : Compute the registration matrix.
 */
class SERVICE_CLASS_API IRegisterer : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IRegisterer, service::IService);

    /**
     * @name Slots API
     * @{
     */
    SERVICE_API static const core::com::Slots::SlotKeyType s_COMPUTE_REGISTRATION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    SERVICE_API IRegisterer() noexcept;

    /**
     * @brief Destructor.
     */
    SERVICE_API ~IRegisterer() noexcept override;

protected:

    /// Register matrix slot
    SERVICE_API virtual void computeRegistration(core::HiResClock::HiResClockType timestamp) = 0;
};

} // namespace sight::service
