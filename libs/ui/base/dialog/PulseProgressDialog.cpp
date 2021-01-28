/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/base/dialog/PulseProgressDialog.hpp"

#include <services/registry/ActiveWorkers.hpp>

namespace sight::ui::base
{
namespace dialog
{
//-----------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog(
    const std::string& title,
    ui::base::dialog::IPulseProgressDialog::Stuff stuff,
    const std::string& msg,
    ui::base::dialog::IPulseProgressDialog::MilliSecond frequenceRefresh )
{

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function<void()>(
                                                                              [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(IPulseProgressDialog::REGISTRY_KEY);
                m_implementation = ui::base::dialog::IPulseProgressDialog::dynamicCast(guiObj);
                if (m_implementation)
                {
                    m_implementation->setStuff(stuff);
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(msg);
                    m_implementation->setFrequence(frequenceRefresh);
                }
                else
                {
                    this->setStuff(stuff);
                }
            } )).wait();
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string& title)
{
    if (m_implementation)
    {
        m_implementation->setTitle(title);
    }
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string& msg)
{
    if (m_implementation)
    {
        m_implementation->setMessage(msg);
    }
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    if (m_implementation)
    {
        services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
            std::bind(&IPulseProgressDialog::show, m_implementation)).wait();
    }
    else
    {
        m_stuff();
    }
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace sight::ui::base
