/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arServices/IRegisterer.hpp"

#include <fwCom/Slots.hxx>

namespace arServices
{

// -----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IRegisterer::s_COMPUTE_REGISTRATION_SLOT = "computeRegistration";

// ----------------------------------------------------------------------------

IRegisterer::IRegisterer() throw ()
{
    newSlot(s_COMPUTE_REGISTRATION_SLOT, &IRegisterer::computeRegistration, this);
}

// ----------------------------------------------------------------------------

IRegisterer::~IRegisterer() throw ()
{
}

// ----------------------------------------------------------------------------

}  // namespace arServices
