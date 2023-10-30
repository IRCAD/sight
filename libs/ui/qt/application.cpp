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

#include "ui/qt/application.hpp"

#include <core/runtime/exit_exception.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/macros.hpp>

#include <QCoreApplication>

SIGHT_REGISTER_GUI(sight::ui::qt::application, sight::ui::application::REGISTRY_KEY);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

void application::exit(int _return_code, bool _async)
{
    const auto& confirm =
        []
        {
            ui::dialog::message dlg;
            dlg.set_title("Confirm exit");
            dlg.set_message("Do you really want to exit?");
            dlg.set_icon(dialog::message::question);
            dlg.add_button(dialog::message::yes_no);

            return dlg.show() == dialog::message::yes;
        };

    if(!m_confirm || confirm())
    {
        if(_async)
        {
            QCoreApplication::exit(_return_code);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(core::runtime::exit_exception(_return_code));
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
