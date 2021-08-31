/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "ui/qml/dialog/InputDialog.hpp"

#include <core/runtime/operations.hpp>

#include <ui/base/registry/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QGuiApplication>
#include <QObject>

fwGuiRegisterMacro(::sight::ui::qml::dialog::InputDialog, ::sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);

namespace sight::ui::qml
{

namespace dialog
{

//------------------------------------------------------------------------------

InputDialog::InputDialog(ui::base::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//------------------------------------------------------------------------------

void InputDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void InputDialog::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void InputDialog::setInput(const std::string& text)
{
    m_input = QString::fromStdString(text);
}

//------------------------------------------------------------------------------

std::string InputDialog::getInput()
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();
    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath =
        core::runtime::getLibraryResourceFilePath("ui_qml/dialog/InputDialog.qml");

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("inputDialog", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialogPath, context);
    SIGHT_ASSERT("The Qml File InputDialog is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    Q_EMIT messageChanged();
    Q_EMIT inputChanged();
    QObject* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);
    m_input = "";

    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    loop.exec();

    delete window;
    return m_input.toStdString();
}

//------------------------------------------------------------------------------

void InputDialog::resultDialog(const QVariant& msg, bool isOk)
{
    if(isOk)
    {
        m_input = msg.toString();
    }
    else
    {
        m_input = "";
    }
}

//------------------------------------------------------------------------------

} // namespace dialog

} // namespace sight::ui::qml
