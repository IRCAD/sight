/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SAction.hpp"

#include <core/runtime/helper.hpp>
namespace sight::module::ui::base
{

static const core::com::Signals::SignalKeyType s_CLICKED_SIG = "clicked";

//-----------------------------------------------------------------------------

SAction::SAction() noexcept
{
    newSignal<IAction::void_signal_t>(s_CLICKED_SIG);
}

//------------------------------------------------------------------------------

void SAction::configuring()
{
    this->initialize();

    auto config = this->getConfiguration();
    m_sync = core::runtime::get_ptree_value(config, "sync", m_sync);
}

//-----------------------------------------------------------------------------

void SAction::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SAction::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SAction::updating()
{
    if(this->confirmAction())
    {
        auto sig = this->signal<void_signal_t>(s_CLICKED_SIG);
        if(m_sync)
        {
            sig->emit();
        }
        else
        {
            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base
