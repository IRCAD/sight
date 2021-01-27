/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "guiQt/Application.hpp"

#include <gui/dialog/IMessageDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/registry/macros.hpp>

#include <QCoreApplication>

fwGuiRegisterMacro( ::sight::guiQt::Application, ::sight::gui::IApplication::REGISTRY_KEY );

namespace sight::guiQt
{

//-----------------------------------------------------------------------------

void Application::exit(int returncode)
{
    if(m_confirm)
    {
        gui::dialog::MessageDialog dlg;
        dlg.setTitle("Confirm exit");
        dlg.setMessage("Do you really want to exit?");
        dlg.setIcon(gui::dialog::IMessageDialog::QUESTION);
        dlg.addButton(gui::dialog::IMessageDialog::YES_NO);

        gui::dialog::IMessageDialog::Buttons res = dlg.show();

        if(res == gui::dialog::IMessageDialog::YES)
        {
            if (QCoreApplication::instance())
            {
                QCoreApplication::instance()->exit(returncode);
            }
        }
    }
    else
    {
        if (QCoreApplication::instance())
        {
            QCoreApplication::instance()->exit(returncode);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::guiQt
