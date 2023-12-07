/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/dialog/logger.hpp"

#include "ui/qml/model/role_table_model.hpp"

#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>
#include <ui/qml/qml_engine.hpp>

#include <QGuiApplication>

#include <filesystem>

SIGHT_REGISTER_GUI(sight::ui::qml::dialog::logger, sight::ui::dialog::logger_base::REGISTRY_KEY);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

void logger::set_title(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void logger::set_message(const std::string& _message)
{
    m_message = QString::fromStdString(_message);
}

//------------------------------------------------------------------------------

void logger::set_logger(const core::log::logger::sptr& _logger)
{
    m_logger = _logger;
}

//------------------------------------------------------------------------------

bool logger::show()
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();
    // the model to set the data of a table
    ui::qml::model::role_table_model model;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path =
        core::runtime::get_library_resource_file_path("ui_qml/dialog/logger.qml");
    // set the root context for the model
    engine->get_root_context()->setContextProperty("loggerModel", &model);

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->get_root_context()));
    context->setContextProperty("logger", this);
    // load the qml ui component
    QObject* window = engine->create_component(dialog_path, context);
    SIGHT_ASSERT("The Qml File logger is not found or not loaded", window);
    // keep window to destroy it
    window->setProperty("title", m_title);

    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);

    // set the icon of the biggest type of error
    auto information =
        core::runtime::get_library_resource_file_path("fwGuiQml/information.svg");
    SIGHT_ASSERT("The information svg is not found", std::filesystem::exists(information));
    auto warning =
        core::runtime::get_library_resource_file_path("fwGuiQml/warning.svg");
    SIGHT_ASSERT("The warning svg is not found", std::filesystem::exists(warning));
    auto critical =
        core::runtime::get_library_resource_file_path("fwGuiQml/critical.svg");
    SIGHT_ASSERT("The critical svg is not found", std::filesystem::exists(critical));
    if(m_logger->count(core::log::log::critical) > 0)
    {
        emit_icon(QUrl::fromLocalFile(QString::fromStdString(critical.string())));
    }
    else if(m_logger->count(core::log::log::warning) > 0)
    {
        emit_icon(QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    }
    else
    {
        emit_icon(QUrl::fromLocalFile(QString::fromStdString(information.string())));
    }

    // Create message
    std::stringstream ss;
    ss << m_message.toStdString()
    << "<br><br>" << "<b>Log report :</b> " << m_logger->count(core::log::log::critical) << " critical, "
    << m_logger->count(core::log::log::warning) << " warning and "
    << m_logger->count(core::log::log::information) << " information messages.";
    emit_message(QString::fromStdString(ss.str()));

    // get the icon of the details checkbox
    auto detailshidden =
        core::runtime::get_library_resource_file_path("fwGuiQml/details-hidden.svg");
    SIGHT_ASSERT("The details-hidden svg is not found", std::filesystem::exists(detailshidden));
    auto detailsshown =
        core::runtime::get_library_resource_file_path("fwGuiQml/details-shown.svg");
    SIGHT_ASSERT("The details-shown svg is not found", std::filesystem::exists(detailsshown));
    emit_hidden(QUrl::fromLocalFile(QString::fromStdString(detailshidden.string())));
    emit_shown(QUrl::fromLocalFile(QString::fromStdString(detailsshown.string())));

    dialog->setProperty("information", QUrl::fromLocalFile(QString::fromStdString(information.string())));
    dialog->setProperty("warning", QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    dialog->setProperty("critical", QUrl::fromLocalFile(QString::fromStdString(critical.string())));

    // Fill log table
    auto it = m_logger->begin();
    model.add_role(Qt::UserRole + 1, "level");
    model.add_role(Qt::UserRole + 2, "message");
    for( ; it != m_logger->end() ; ++it)
    {
        QString level_string = "Unkown";
        QHash<QByteArray, QVariant> data;
        core::log::log::level_t level = it->get_level();
        if(level == core::log::log::information)
        {
            level_string = "Information";
        }
        else if(level == core::log::log::warning)
        {
            level_string = "Warning";
        }
        else if(level == core::log::log::critical)
        {
            level_string = "Critical";
        }

        data.insert("level", level_string);
        data.insert("message", QString::fromStdString(it->get_message()));
        model.add_data(QHash<QByteArray, QVariant>(data));
    }

    SIGHT_ASSERT("The Logger need at least one error", !model.is_empty());

    m_is_ok = false;
    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));

    [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(dialog, "open");
    SIGHT_ASSERT("The slot `open` was not found.", ok);

    loop.exec();

    delete window;
    return m_is_ok;
}

//------------------------------------------------------------------------------

void logger::result_dialog(bool _is_ok)
{
    m_is_ok = _is_ok;
}

//------------------------------------------------------------------------------

void logger::emit_hidden(const QUrl& _hidden)
{
    m_hidden = _hidden;
    Q_EMIT hidden_changed();
}

//------------------------------------------------------------------------------

void logger::emit_icon(const QUrl& _path)
{
    m_icon = _path;
    Q_EMIT icon_changed();
}

//------------------------------------------------------------------------------

void logger::emit_message(const QString& _message)
{
    m_message = _message;
    Q_EMIT message_changed();
}

//------------------------------------------------------------------------------

void logger::emit_shown(const QUrl& _shown)
{
    m_shown = _shown;
    Q_EMIT shown_changed();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
