/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "signal.hpp"

#include <core/com/signal.hxx>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::ui::com
{

//-----------------------------------------------------------------------------

static const core::com::signals::key_t TRIGGERED_SIG = "triggered";
static const core::com::signals::key_t CANCELLED_SIG = "cancelled";

//-----------------------------------------------------------------------------

signal::signal() noexcept :
    m_sigTriggered(new_signal<TriggeredSignalType>(TRIGGERED_SIG)),
    m_sigCancelled(new_signal<TriggeredSignalType>(CANCELLED_SIG))
{
    SIGHT_WARN(
        "'sight::module::ui::com::signal' is deprecated and will be removed in Sight 24.0,"
        " please use 'action' instead."
    );
}

//-----------------------------------------------------------------------------

signal::~signal() noexcept =
    default;

//-----------------------------------------------------------------------------

void signal::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void signal::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void signal::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void signal::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void signal::updating()
{
    if(this->confirmAction())
    {
        m_sigTriggered->async_emit(this->checked());
    }
    else
    {
        m_sigCancelled->async_emit(this->checked());
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::com
