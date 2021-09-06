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

#include "ui/qml/dialog/LoggerDialog.hpp"

#include "ui/qml/model/RoleTableModel.hpp"

#include <core/runtime/operations.hpp>

#include <ui/base/registry/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QGuiApplication>

#include <filesystem>

fwGuiRegisterMacro(::sight::ui::qml::dialog::LoggerDialog, ::sight::ui::base::dialog::ILoggerDialog::REGISTRY_KEY);

namespace sight::ui::qml
{

namespace dialog
{

//------------------------------------------------------------------------------

LoggerDialog::LoggerDialog(ui::base::GuiBaseObject::Key)
{
}

//------------------------------------------------------------------------------

LoggerDialog::~LoggerDialog()
{
}

//------------------------------------------------------------------------------

void LoggerDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_message = QString::fromStdString(message);
}

//------------------------------------------------------------------------------

void LoggerDialog::setLogger(const core::log::Logger::sptr& logger)
{
    m_logger = logger;
}

//------------------------------------------------------------------------------

bool LoggerDialog::show()
{
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();
    // the model to set the data of a table
    ui::qml::model::RoleTableModel model;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath =
        core::runtime::getLibraryResourceFilePath("ui_qml/dialog/LoggerDialog.qml");
    // set the root context for the model
    engine->getRootContext()->setContextProperty("loggerModel", &model);

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("loggerDialog", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialogPath, context);
    SIGHT_ASSERT("The Qml File LoggerDialog is not found or not loaded", window);
    // keep window to destroy it
    window->setProperty("title", m_title);

    QObject* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);

    // set the icon of the biggest type of error
    auto information =
        core::runtime::getLibraryResourceFilePath("fwGuiQml/information.svg");
    SIGHT_ASSERT("The information svg is not found", std::filesystem::exists(information));
    auto warning =
        core::runtime::getLibraryResourceFilePath("fwGuiQml/warning.svg");
    SIGHT_ASSERT("The warning svg is not found", std::filesystem::exists(warning));
    auto critical =
        core::runtime::getLibraryResourceFilePath("fwGuiQml/critical.svg");
    SIGHT_ASSERT("The critical svg is not found", std::filesystem::exists(critical));
    if(m_logger->count(core::log::Log::CRITICAL) > 0)
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(critical.string())));
    }
    else if(m_logger->count(core::log::Log::WARNING) > 0)
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    }
    else
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(information.string())));
    }

    // Create message
    std::stringstream ss;
    ss << m_message.toStdString()
    << "<br><br>" << "<b>Log report :</b> " << m_logger->count(core::log::Log::CRITICAL) << " critical, "
    << m_logger->count(core::log::Log::WARNING) << " warning and "
    << m_logger->count(core::log::Log::INFORMATION) << " information messages.";
    emitMessage(QString::fromStdString(ss.str()));

    // get the icon of the details checkbox
    auto detailshidden =
        core::runtime::getLibraryResourceFilePath("fwGuiQml/details-hidden.svg");
    SIGHT_ASSERT("The details-hidden svg is not found", std::filesystem::exists(detailshidden));
    auto detailsshown =
        core::runtime::getLibraryResourceFilePath("fwGuiQml/details-shown.svg");
    SIGHT_ASSERT("The details-shown svg is not found", std::filesystem::exists(detailsshown));
    emitHidden(QUrl::fromLocalFile(QString::fromStdString(detailshidden.string())));
    emitShown(QUrl::fromLocalFile(QString::fromStdString(detailsshown.string())));

    dialog->setProperty("information", QUrl::fromLocalFile(QString::fromStdString(information.string())));
    dialog->setProperty("warning", QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    dialog->setProperty("critical", QUrl::fromLocalFile(QString::fromStdString(critical.string())));

    // Fill log table
    core::log::Logger::ConstIteratorType it = m_logger->begin();
    model.addRole(Qt::UserRole + 1, "level");
    model.addRole(Qt::UserRole + 2, "message");
    for( ; it != m_logger->end() ; ++it)
    {
        QString levelString = "Unkown";
        QHash<QByteArray, QVariant> data;
        core::log::Log::LevelType level = it->getLevel();
        if(level == core::log::Log::INFORMATION)
        {
            levelString = "Information";
        }
        else if(level == core::log::Log::WARNING)
        {
            levelString = "Warning";
        }
        else if(level == core::log::Log::CRITICAL)
        {
            levelString = "Critical";
        }

        data.insert("level", levelString);
        data.insert("message", QString::fromStdString(it->getMessage()));
        model.addData(QHash<QByteArray, QVariant>(data));
    }

    SIGHT_ASSERT("The Logger need at least one error", !model.isEmpty());

    m_isOk = false;
    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    loop.exec();

    delete window;
    return m_isOk;
}

//------------------------------------------------------------------------------

void LoggerDialog::resultDialog(bool isOk)
{
    m_isOk = isOk;
}

//------------------------------------------------------------------------------

void LoggerDialog::emitHidden(const QUrl& hidden)
{
    m_hidden = hidden;
    Q_EMIT hiddenChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitIcon(const QUrl& path)
{
    m_icon = path;
    Q_EMIT iconChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitMessage(const QString& message)
{
    m_message = message;
    Q_EMIT messageChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitShown(const QUrl& shown)
{
    m_shown = shown;
    Q_EMIT shownChanged();
}

//------------------------------------------------------------------------------

} // namespace dialog

} // namespace sight::ui::qml
