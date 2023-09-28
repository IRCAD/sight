/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "service/base.hpp"
#include "service/config.hpp"

namespace sight::service
{

/**
 * @brief   This interface defines registerer service API.
 *          Must be implemented for services that register objects together.
 *
 * @section Slots Slots
 * - \b computeRegistration(core::hires_clock::type) : Compute the registration matrix.
 */
class SERVICE_CLASS_API registerer : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(registerer, service::base);

    /**
     * @name Slots API
     * @{
     */
    SERVICE_API static const core::com::slots::key_t COMPUTE_REGISTRATION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    SERVICE_API registerer() noexcept;

    /**
     * @brief Destructor.
     */
    SERVICE_API ~registerer() noexcept override;

protected:

    /// Register matrix slot
    SERVICE_API virtual void computeRegistration(core::hires_clock::type timestamp) = 0;
};

} // namespace sight::service
