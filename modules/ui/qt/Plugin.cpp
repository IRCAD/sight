/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/Plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/thread/ActiveWorkers.hpp>

#include <service/macros.hpp>

#include <ui/qt/App.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <QFile>
#include <QResource>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include <functional>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("::sight::module::ui::qt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    core::runtime::Profile::sptr profile = core::runtime::getCurrentProfile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QApplication>(new ::sight::ui::qt::App(argc, argv, true));
        };

    m_workerQt = ::sight::ui::qt::getQtWorker(argc, argv, callback, profile->getName(), profile->getVersion());

    core::thread::ActiveWorkers::setDefaultWorker(m_workerQt);

    m_workerQt->post(std::bind(&Plugin::loadStyleSheet, this));

    core::runtime::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(m_workerQt)
    {
        m_workerQt->stop();
        m_workerQt.reset();
    }
}

//-----------------------------------------------------------------------------

void setup()
{
    core::runtime::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post(std::bind(&setup));
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    core::runtime::getCurrentProfile()->cleanup();
    int result = std::any_cast<int>(m_workerQt->getFuture().get());

    core::thread::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if(this->getModule()->hasParameter("resource"))
    {
        const std::string resourceFile = this->getModule()->getParameterValue("resource");
        const auto path                = core::runtime::getModuleResourceFilePath(resourceFile);

        const bool resourceLoaded = QResource::registerResource(path.string().c_str());
        SIGHT_ASSERT("Cannot load resources '" + resourceFile + "'.", resourceLoaded);
    }

    if(this->getModule()->hasParameter("style"))
    {
        const std::string style = this->getModule()->getParameterValue("style");
        qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
    }

    if(this->getModule()->hasParameter("stylesheet"))
    {
        const std::string stylesheetFile = this->getModule()->getParameterValue("stylesheet");
        const auto path                  = core::runtime::getModuleResourceFilePath(stylesheetFile);

        QFile data(QString::fromStdString(path.string()));
        QString style;
        if(data.open(QFile::ReadOnly))
        {
            QTextStream styleIn(&data);
            style = styleIn.readAll();
            data.close();
            qApp->setStyleSheet(style);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
