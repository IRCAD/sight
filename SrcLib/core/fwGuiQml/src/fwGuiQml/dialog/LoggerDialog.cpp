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

#include "fwGuiQml/dialog/LoggerDialog.hpp"

#include "fwGuiQml/model/RoleTableModel.hpp"

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <QGuiApplication>

fwGuiRegisterMacro( ::fwGuiQml::dialog::LoggerDialog, ::fwGui::dialog::ILoggerDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

LoggerDialog::LoggerDialog(::fwGui::GuiBaseObject::Key key)
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

void LoggerDialog::setLogger(const ::fwLog::Logger::sptr& logger)
{
    m_logger = logger;
}

//------------------------------------------------------------------------------

bool LoggerDialog::show()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // the model to set the data of a table
    ::fwGuiQml::model::RoleTableModel model;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/LoggerDialog.qml");
    // set the root context for the model
    engine->getRootContext()->setContextProperty("loggerModel", &model);

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("loggerDialog", this);
    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);
    // keep window to destroy it
    QObject* window = dialog;

    dialog->setProperty("title", m_title);

    dialog = dialog->findChild<QObject*>("dialog");

    // set the icon of the biggest type of error
    auto information =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/information.svg");
    if (!::boost::filesystem::exists(information))
    {
        information = "";
    }
    auto warning =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/warning.svg");
    if (!::boost::filesystem::exists(warning))
    {
        warning = "";
    }
    auto critical =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/critical.svg");
    if (!::boost::filesystem::exists(critical))
    {
        critical = "";
    }
    if (m_logger->count(::fwLog::Log::CRITICAL) > 0)
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(critical.string())));
    }
    else if (m_logger->count(::fwLog::Log::WARNING) > 0)
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    }
    else
    {
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(information.string())));
    }
    // Create message
    std::stringstream ss;
    ss << m_message.toStdString() <<
        "<br><br>" << "<b>Log report :</b> " << m_logger->count(::fwLog::Log::CRITICAL) << " critical, " <<
        m_logger->count(::fwLog::Log::WARNING) << " warning and " <<
        m_logger->count(::fwLog::Log::INFORMATION) << " information messages.";
    emitMessage(QString::fromStdString(ss.str()));

    // get the icon of the details checkbox
    auto detailshidden =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/details-hidden.svg");
    if (!::boost::filesystem::exists(detailshidden))
    {
        detailshidden = "";
    }
    auto detailsshown =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/details-shown.svg");
    if (!::boost::filesystem::exists(detailsshown))
    {
        detailsshown = "";
    }
    emitHidden(QUrl::fromLocalFile(QString::fromStdString(detailshidden.string())));
    emitShown(QUrl::fromLocalFile(QString::fromStdString(detailsshown.string())));

    dialog->setProperty("information", QUrl::fromLocalFile(QString::fromStdString(information.string())));
    dialog->setProperty("warning", QUrl::fromLocalFile(QString::fromStdString(warning.string())));
    dialog->setProperty("critical", QUrl::fromLocalFile(QString::fromStdString(critical.string())));

    // Fill log table
    ::fwLog::Logger::ConstIteratorType it = m_logger->begin();
    model.addRole(Qt::UserRole + 1, "level");
    model.addRole(Qt::UserRole + 2, "message");
    for(; it != m_logger->end(); ++it)
    {
        QString levelString = "Unkown";
        QHash<QByteArray, QVariant> data;
        ::fwLog::Log::LevelType level = it->getLevel();
        if (level == ::fwLog::Log::INFORMATION)
        {
            levelString = "Information";
        }
        else if (level == ::fwLog::Log::WARNING)
        {
            levelString = "Warning";
        }
        else if (level == ::fwLog::Log::CRITICAL)
        {
            levelString = "Critical";
        }
        data.insert("level", levelString);
        data.insert("message", QString::fromStdString(it->getMessage()));
        model.addData(QHash<QByteArray, QVariant>(data));
    }
    SLM_ASSERT("The Logger need at least one error", !model.isEmpty());
    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
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
} // namespace fwGuiQml
