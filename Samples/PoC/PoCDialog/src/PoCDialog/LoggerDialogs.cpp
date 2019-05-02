#include "PoCDialog/LoggerDialogs.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QObject>

LoggerDialogs::LoggerDialogs()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // add context for root Context
    engine->getRootContext()->setContextProperty("pocDialogLoggerDialogs", this);
}

//------------------------------------------------------------------------------

LoggerDialogs::~LoggerDialogs()
{
}

//------------------------------------------------------------------------------

void LoggerDialogs::open()
{
    ::fwLog::Logger::sptr logger = ::fwLog::Logger::New();

    // Fill Logger
    for (int i = 0; i < m_errorLevel.size(); ++i)
    {
        auto level      = m_errorLevel.at(i).toStdString();
        auto message = m_errorMessage.at(i).toStdString();
        if (level == "Critical")
        {
            logger->critical(message);
        }
        else if (level == "Warning")
        {
            logger->warning(message);
        }
        else if (level == "Information")
        {
            logger->information(message);
        }
    }
    ::fwGui::dialog::LoggerDialog dialogFile(m_title.toStdString(), m_message.toStdString(), logger);
    dialogFile.show();
}
