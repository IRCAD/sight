/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arServices/IOperator.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

namespace arServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType IOperator::s_COMPUTED_SIG = "computed";

const ::fwCom::Slots::SlotKeyType IOperator::s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType IOperator::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType IOperator::s_SET_INT_PARAMETER_SLOT    = "setIntParameter";

//-----------------------------------------------------------------------------

IOperator::IOperator()
{
    m_sigComputed = newSignal<ComputedSignalType>(s_COMPUTED_SIG);

    newSlot(s_SET_BOOL_PARAMETER_SLOT, &IOperator::setBoolParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &IOperator::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &IOperator::setIntParameter, this);
}

//-----------------------------------------------------------------------------

IOperator::~IOperator()
{
}

//-----------------------------------------------------------------------------

void IOperator::setBoolParameter(std::string key, bool val)
{

}

//-----------------------------------------------------------------------------

void IOperator::setDoubleParameter(std::string key, double val)
{

}

//-----------------------------------------------------------------------------

void IOperator::setIntParameter(std::string key, int val)
{

}

//-----------------------------------------------------------------------------

}
