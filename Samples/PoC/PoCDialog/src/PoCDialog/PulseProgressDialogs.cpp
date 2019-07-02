/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "PoCDialog/PulseProgressDialogs.hpp"

#include <fwGui/dialog/PulseProgressDialog.hpp>

#include <fwQml/QmlEngine.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

PulseProgressDialogs::PulseProgressDialogs()
{
}

//------------------------------------------------------------------------------

PulseProgressDialogs::~PulseProgressDialogs()
{
}

//------------------------------------------------------------------------------

void poCTest()
{
    int i = 0;
    while (i++ < 100)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

//------------------------------------------------------------------------------

void PulseProgressDialogs::open()
{
    std::function<void()> test = poCTest;
    ::fwGui::dialog::PulseProgressDialog dialog(m_title.toStdString(), test, m_message.toStdString());
    dialog.show();
}
