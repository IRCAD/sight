/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IRegisterer.hpp"

#include <fwCom/Slots.hxx>

namespace fwServices
{

// -----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IRegisterer::s_COMPUTE_REGISTRATION_SLOT = "computeRegistration";

// ----------------------------------------------------------------------------

IRegisterer::IRegisterer() noexcept
{
    newSlot(s_COMPUTE_REGISTRATION_SLOT, &IRegisterer::computeRegistration, this);
}

// ----------------------------------------------------------------------------

IRegisterer::~IRegisterer() noexcept
{
}

// ----------------------------------------------------------------------------

}  // namespace fwServices
