/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QInputDialog>
#include <QObject>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <fwGui/registry/macros.hpp>

#include "fwGuiQt/dialog/InputDialog.hpp"


fwGuiRegisterMacro( ::fwGuiQt::dialog::InputDialog, ::fwGui::dialog::IInputDialog::REGISTRY_KEY );

namespace fwGuiQt
{
namespace dialog
{

//------------------------------------------------------------------------------

InputDialog::InputDialog(::fwGui::GuiBaseObject::Key key) : m_input("")
{}

//------------------------------------------------------------------------------

InputDialog::~InputDialog()
{}

//------------------------------------------------------------------------------

void InputDialog::setTitle( const std::string &title )
{
    m_title = title;
}

//------------------------------------------------------------------------------

void InputDialog::setMessage( const std::string &msg )
{
    m_message = msg;
}

//------------------------------------------------------------------------------

/// Set the input text in the input field
void InputDialog::setInput(const std::string &text)
{
    m_input = text;
}

//------------------------------------------------------------------------------

/// Get the input text in the input field
std::string InputDialog::getInput()
{
    QString title = QObject::tr(m_title.c_str());
    QString text = QObject::tr(m_message.c_str());

    bool IsOkClicked;
    QString outputText = QInputDialog::getText(qApp->activeWindow(), title, text, QLineEdit::Normal,QString::fromStdString(m_input),&IsOkClicked);

    if ( IsOkClicked)
    {
        m_input = outputText.toStdString();
    }
    else
    {
        m_input = "";
    }
    return m_input;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQt



