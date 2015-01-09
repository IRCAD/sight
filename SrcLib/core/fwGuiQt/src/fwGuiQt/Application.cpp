/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>

#include <fwGui/dialog/IMessageDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include "fwGuiQt/Application.hpp"


fwGuiRegisterMacro(::fwGuiQt::Application, ::fwGui::IApplication::REGISTRY_KEY );

namespace fwGuiQt
{

//-----------------------------------------------------------------------------

void Application::exit(int returncode)
{
    if(m_confirm)
    {
        ::fwGui::dialog::MessageDialog dlg;
        dlg.setTitle("Confirm exit");
        dlg.setMessage("Do you really want to exit?");
        dlg.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        dlg.addButton(::fwGui::dialog::IMessageDialog::YES_NO);

        ::fwGui::dialog::IMessageDialog::Buttons res = dlg.show();

        if(res == ::fwGui::dialog::IMessageDialog::YES)
        {
            qApp->exit(returncode);
        }
    }
    else
    {
        qApp->exit(returncode);
    }
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQt



