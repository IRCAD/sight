/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/__/dialog/pulse_progress_base.hpp"

namespace sight::ui::dialog
{

const pulse_progress_base::FactoryRegistryKeyType pulse_progress_base::REGISTRY_KEY =
    "::ui::dialog::pulse_progress";

pulse_progress_base::pulse_progress_base()
= default;

//-----------------------------------------------------------------------------

pulse_progress_base::~pulse_progress_base()
= default;

//-----------------------------------------------------------------------------

void pulse_progress_base::setStuff(Stuff stuff)
{
    m_stuff = stuff;
}

//-----------------------------------------------------------------------------

void pulse_progress_base::setFrequence(MilliSecond frequence)
{
    m_frequence = frequence;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
