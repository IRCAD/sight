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

#include "fwGuiQml/dialog/LoggerDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

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
    m_title = title;
}

//------------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_message = message;
}

//------------------------------------------------------------------------------

void LoggerDialog::setLogger(const ::fwLog::Logger::sptr& logger)
{
    m_logger = logger;
}

//------------------------------------------------------------------------------

bool LoggerDialog::show()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/LoggerDialog.qml");

    // load the qml ui component
    m_dialog = engine->createComponent(dialogPath);
    QObject::connect(m_dialog, SIGNAL(resultDialog(bool)),
                     this, SLOT(resultDialog(bool)));
    m_dialog->setProperty("title", QString::fromStdString(m_title));

    QObject* icon = m_dialog->findChild<QObject*>("icon");

    if (m_logger->count(::fwLog::Log::CRITICAL) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png");
        icon->setProperty("source", QUrl(QString::fromUtf8(path.c_str())));
    }
    else if (m_logger->count(::fwLog::Log::WARNING) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png");
        icon->setProperty("source", QUrl(QString::fromUtf8(path.c_str())));
    }
    else
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png");
        icon->setProperty("source", QUrl(QString::fromUtf8(path.c_str())));
    }
    // Create message
    std::stringstream ss;
    ss << m_message <<
        "<br><br>" << "<b>Log report :</b> " << m_logger->count(::fwLog::Log::CRITICAL) << " critical, " <<
        m_logger->count(::fwLog::Log::WARNING) << " warning and " <<
        m_logger->count(::fwLog::Log::INFORMATION) << " information messages.";
    QObject* description = m_dialog->findChild<QObject*>("description");
    description->setProperty("text", QString::fromStdString(ss.str()));

    const auto detailshidden = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-hidden.png").string();
    const auto detailsshown  = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-shown.png").string();
    m_dialog->setProperty("hidden", QUrl(QString::fromUtf8(detailshidden.c_str())));
    m_dialog->setProperty("shown", QUrl(QString::fromUtf8(detailsshown.c_str())));

    const auto information = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png");
    const auto warning     = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png");
    const auto critical    = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png");
    m_dialog->setProperty("information", QUrl(QString::fromUtf8(information.c_str())));
    m_dialog->setProperty("warning", QUrl(QString::fromUtf8(warning.c_str())));
    m_dialog->setProperty("critical", QUrl(QString::fromUtf8(critical.c_str())));

    // Fill log table
    QObject* table = m_dialog->findChild<QObject*>("errorModel");
    ::fwLog::Logger::ConstIteratorType it = m_logger->begin();
    for(; it != m_logger->end(); ++it)
    {
        QString levelString = "Unkown";
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
        QMetaObject::invokeMethod(table, "addError",
                                  Q_ARG(QVariant, levelString),
                                  Q_ARG(QVariant, QString::fromStdString(it->getMessage())));
    }
    QMetaObject::invokeMethod(m_dialog, "open");

    while (!m_isClicked && m_dialog->property("visible").toBool())
    {
        qGuiApp->processEvents();
    }
    delete m_dialog;
    return m_isOk;
}

//------------------------------------------------------------------------------

void LoggerDialog::resultDialog(bool isOk)
{
    m_isOk      = isOk;
    m_isClicked = true;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
