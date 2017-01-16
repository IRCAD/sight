/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSERVICES_IREGISTERER_HPP__
#define __ARSERVICES_IREGISTERER_HPP__

#include "arServices/config.hpp"

#include <fwServices/IService.hpp>

namespace arServices
{
/**
 * @brief   This interface defines registerer service API.
 *          Must be implemented for services that register objects together.
 *
 * @section Slots Slots
 * - \b computeMatrix(::fwCore::HiResClock::HiResClockType) : Compute the registration matrix.
 */
class ARSERVICES_CLASS_API IRegisterer : public fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro((IRegisterer)(fwServices::IService));

    /**
     * @name Slots API
     * @{
     */
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_MATRIX_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    ARSERVICES_API IRegisterer() throw ();

    /**
     * @brief Destructor.
     */
    ARSERVICES_API virtual ~IRegisterer() throw ();

protected:

    /// Register matrix slot
    ARSERVICES_API virtual void computeMatrix(::fwCore::HiResClock::HiResClockType timestamp) = 0;
};

} // namespace arServices
#endif  // __ARSERVICES_IREGISTERER_HPP__
