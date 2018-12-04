/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{
/**
 * @brief   This interface defines registerer service API.
 *          Must be implemented for services that register objects together.
 *
 * @section Slots Slots
 * - \b computeRegistration(::fwCore::HiResClock::HiResClockType) : Compute the registration matrix.
 */
class FWSERVICES_CLASS_API IRegisterer : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro((IRegisterer)(::fwServices::IService));

    /**
     * @name Slots API
     * @{
     */
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_REGISTRATION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    FWSERVICES_API IRegisterer() noexcept;

    /**
     * @brief Destructor.
     */
    FWSERVICES_API virtual ~IRegisterer() noexcept;

protected:

    /// Register matrix slot
    FWSERVICES_API virtual void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) = 0;
};

} // namespace fwServices
