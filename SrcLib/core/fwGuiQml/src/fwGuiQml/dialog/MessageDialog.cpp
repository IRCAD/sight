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

#include "fwGuiQml/dialog/MessageDialog.hpp"

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QVector>

fwGuiRegisterMacro( ::fwGuiQml::dialog::MessageDialog, ::fwGui::dialog::IMessageDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

//value of the enum in integer
typedef const std::map< ::fwGui::dialog::IMessageDialog::Icons, int> MessageDialogQmlIconsType;
MessageDialogQmlIconsType messageDialogQmlIcons =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::NONE, 0)
        (::fwGui::dialog::IMessageDialog::QUESTION, 4)
        (::fwGui::dialog::IMessageDialog::INFO, 1)
        (::fwGui::dialog::IMessageDialog::WARNING, 2)
        (::fwGui::dialog::IMessageDialog::CRITICAL, 3);

typedef const std::map< ::fwGui::dialog::IMessageDialog::Buttons,
                        QMessageBox::StandardButton> MessageDialogQmlButtonType;
MessageDialogQmlButtonType messageDialogQmlButton =
    ::boost::assign::map_list_of(::fwGui::dialog::IMessageDialog::OK, QMessageBox::Ok)
        (::fwGui::dialog::IMessageDialog::CANCEL, QMessageBox::Cancel)
        (::fwGui::dialog::IMessageDialog::YES, QMessageBox::Yes)
        (::fwGui::dialog::IMessageDialog::NO, QMessageBox::No);

//------------------------------------------------------------------------------

MessageDialog::MessageDialog(::fwGui::GuiBaseObject::Key key) :
    m_buttons(::fwGui::dialog::IMessageDialog::NOBUTTON),
    m_icon(::fwGui::dialog::IMessageDialog::NONE)
{
}

//------------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//------------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string& title )
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string& msg )
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

//------------------------------------------------------------------------------

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    m_buttons = (::fwGui::dialog::IMessageDialog::Buttons) ( m_buttons | button );
}

//------------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(::fwGui::dialog::IMessageDialog::Buttons button )
{
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQmlIconsType::const_iterator iterIcon = messageDialogQmlIcons.find(m_icon);
    SLM_ASSERT("Unknown Icon", iterIcon != messageDialogQmlIcons.end());
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_isClicked                     = false;
    m_clicked                       = ::fwGui::dialog::IMessageDialog::NOBUTTON;
    int icon = iterIcon->second;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/MessageDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("messageDialog", this);
    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);

    Q_EMIT titleChanged();
    Q_EMIT messageChanged();
    emitIcon(icon);

    emitButtons();
    QMetaObject::invokeMethod(dialog, "open");
    while (!m_isClicked && m_visible)
    {
        qGuiApp->processEvents();
    }
    delete dialog;
    return m_clicked;
}

//------------------------------------------------------------------------------

void MessageDialog::resultDialog(int clicked)
{
    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
    {
        // get the button that has been click
        if ( clicked == button.second)
        {
            m_clicked = button.first;
            break;
        }
    }
    m_isClicked = true;
}

//------------------------------------------------------------------------------

void MessageDialog::emitIcon(const int& icon)
{
    m_iconDialog = icon;
    Q_EMIT iconChanged();
}

//------------------------------------------------------------------------------

void MessageDialog::emitButtons()
{
    QMessageBox::StandardButtons buttons;
    // add the different type of button needed
    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
    {
        if ( m_buttons & button.first)
        {
            buttons |= button.second;
        }
    }
    m_buttonsDialog = buttons;
    Q_EMIT buttonsChanged();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
