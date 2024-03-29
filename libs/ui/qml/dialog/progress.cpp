/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "ui/qml/dialog/progress.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/__/frame.hpp>
#include <ui/__/macros.hpp>
#include <ui/qml/qml_engine.hpp>

#include <QGuiApplication>
#include <QObject>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickWindow>

SIGHT_REGISTER_GUI(
    sight::ui::qml::dialog::progress,
    sight::ui::dialog::progress_base::REGISTRY_KEY
);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

progress::progress(
    const std::string& _title,
    const std::string& _message
)
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();
    // find if toolbar exist on the ApplicationWindow
    const auto& root_objects = engine->get_root_objects();
    QObject* tool_bar        = nullptr;
    for(const auto& root : root_objects)
    {
        tool_bar = root->findChild<QObject*>("fwGuiQml_ProgressBar");
        if(tool_bar != nullptr)
        {
            break;
        }
    }

    // TODO: find a way to remove the context from rootContext but instead only on the object
    engine->get_root_context()->setContextProperty("progressDialog", this);
    if(tool_bar != nullptr)
    {
        // get the path of the qml ui file in the 'rc' directory
        const auto& dialog_path =
            core::runtime::get_library_resource_file_path("ui_qml/dialog/Progress.qml");
        // load the qml ui component
        m_dialog = engine->create_component(dialog_path);
        SIGHT_ASSERT("The Qml File progress is not found or not loaded", m_dialog);
        auto* item = qobject_cast<QQuickItem*>(m_dialog);
        // get ownership to not get Progress qml destroyed
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // set visual parent of progress qml for render
        item->setParent(tool_bar);
        // set the progress qml inside the root
        item->setParentItem(qobject_cast<QQuickItem*>(tool_bar));
    }
    else
    {
        // get the path of the qml ui file in the 'rc' directory
        const auto& dialog_path =
            core::runtime::get_library_resource_file_path("ui_qml/dialog/progress.qml");
        // load the qml ui component
        m_window = engine->create_component(dialog_path);
        SIGHT_ASSERT("The Qml File progress is not found or not loaded", m_window);
        m_window->setProperty("title", QString::fromStdString(_title));
        m_dialog = m_window->findChild<QObject*>("dialog");
        SIGHT_ASSERT("The dialog is not found inside the window", m_dialog);

        [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(m_dialog, "open");
        SIGHT_ASSERT("The slot `open` was not found.", ok);
    }

    m_visible = true;
    this->progress::set_title(_title);
    this->progress::set_message(_message);
}

//------------------------------------------------------------------------------

progress::~progress()
{
    if(m_window != nullptr)
    {
        m_window->deleteLater();
    }
    else
    {
        m_dialog->deleteLater();
    }
}

//------------------------------------------------------------------------------

void progress::operator()(float _percent, std::string _msg)
{
    SIGHT_ASSERT("m_dialog not instanced", m_dialog);
    // check if the dialog box has been closed by the user and cancel the progress
    if(!m_visible)
    {
        if(m_cancel_callback != nullptr)
        {
            this->cancel_pressed();
        }

        return;
    }

    const int& value = static_cast<int>(_percent * 100);
    if(value != this->m_value)
    {
        this->m_value = value;
        this->set_message(_msg);
        this->set_title(m_title.toStdString());
    }
}

//------------------------------------------------------------------------------

void progress::set_title(const std::string& _title)
{
    SIGHT_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);

    m_title = QString::fromStdString(_title);
    if(m_window != nullptr)
    {
        Q_EMIT title_changed();
    }
}

//------------------------------------------------------------------------------

void progress::set_message(const std::string& _msg)
{
    SIGHT_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);
    QString message = "";
    QString title   = m_title;
    if(!title.isEmpty() && (this->m_window == nullptr))
    {
        message += title;
        message += " - ";
    }

    message = message + QString::fromStdString(_msg);
    if(m_visible)
    {
        [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(
            m_dialog,
            "changeValue",
            Q_ARG(QVariant, message),
            Q_ARG(QVariant, qreal(m_value))
        );
        SIGHT_ASSERT("The slot `changeValue` was not found.", ok);
    }
}

//------------------------------------------------------------------------------

void progress::hide_cancel_button()
{
    m_has_callback = false;
    Q_EMIT has_callback_changed();
}

//------------------------------------------------------------------------------

void progress::cancel_pressed()
{
    progress_base::cancel_pressed();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
