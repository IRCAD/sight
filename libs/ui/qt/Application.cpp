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

#include "ui/qt/Application.hpp"

#include <core/runtime/exit_exception.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/macros.hpp>

#include <QCoreApplication>

SIGHT_REGISTER_GUI(sight::ui::qt::Application, sight::ui::application::REGISTRY_KEY);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

void Application::exit(int returnCode, bool async)
{
    const auto& confirm =
        []
        {
            ui::dialog::message dlg;
            dlg.setTitle("Confirm exit");
            dlg.setMessage("Do you really want to exit?");
            dlg.setIcon(dialog::message::QUESTION);
            dlg.addButton(dialog::message::YES_NO);

            return dlg.show() == dialog::message::YES;
        };

    if(!m_confirm || confirm())
    {
        if(async)
        {
            QCoreApplication::exit(returnCode);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(core::runtime::exit_exception(returnCode));
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
