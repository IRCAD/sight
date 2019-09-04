/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwGuiQt/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/IInputDialog.hpp>

#include <string>

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   ::fwGui::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class FWGUIQT_CLASS_API InputDialog : public ::fwGui::dialog::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::dialog::IInputDialog),
                                            ::fwGui::factory::New< InputDialog > );

    FWGUIQT_API InputDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~InputDialog();

    /// Set the title of the message box
    FWGUIQT_API virtual void setTitle( const std::string& title ) override;

    /// Set the message
    FWGUIQT_API virtual void setMessage( const std::string& msg ) override;

    /// Set the input text in the input field
    FWGUIQT_API virtual void setInput(const std::string& text) override;

    /// Get the input text in the input field
    FWGUIQT_API virtual std::string getInput() override;

protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// Text inputed
    std::string m_input;

};
} // namespace dialog
} // namespace fwGuiQt
