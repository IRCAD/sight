/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arServices/IOperator.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

namespace arServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType IOperator::s_COMPUTED_SIG = "computed";

//-----------------------------------------------------------------------------

IOperator::IOperator() :
    m_sigComputed(ComputedSignalType::New())
{
    ::fwCom::HasSignals::m_signals(s_COMPUTED_SIG, m_sigComputed);


}

//-----------------------------------------------------------------------------

IOperator::~IOperator()
{
}

//-----------------------------------------------------------------------------

}
