/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
