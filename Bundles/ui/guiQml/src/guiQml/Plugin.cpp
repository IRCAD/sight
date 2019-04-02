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

#include "guiQml/Plugin.hpp"

#include <fwCore/base.hpp>

#include <fwGuiQml/App.hpp>
#include <fwGuiQml/WorkerQml.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <QFile>
#include <QResource>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include <functional>

namespace guiQml
{
//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQml::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    m_workerQt = ::fwGuiQml::getQmlWorker(argc, argv);

    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_workerQt);

    m_workerQt->post( std::bind( &Plugin::loadStyleSheet, this ) );

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
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
    ::fwRuntime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post( std::bind( &setup ) );
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    int result = ::boost::any_cast<int>(m_workerQt->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if( this->getBundle()->hasParameter("resource") )
    {
        const std::string resourceFile = this->getBundle()->getParameterValue("resource");
        const auto path                = fwRuntime::getBundleResourceFilePath(resourceFile);

        const bool resourceLoaded = QResource::registerResource(path.string().c_str());
        SLM_ASSERT("Cannot load resources '"+resourceFile+"'.", resourceLoaded);
    }

    if( this->getBundle()->hasParameter("style") )
    {
        const std::string style = this->getBundle()->getParameterValue("style");
        //qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
    }

    if( this->getBundle()->hasParameter("stylesheet") )
    {
        const std::string stylesheetFile = this->getBundle()->getParameterValue("stylesheet");
        const auto path                  = fwRuntime::getBundleResourceFilePath(stylesheetFile);

        QFile data(QString::fromStdString(path.string()));
        QString style;
        if(data.open(QFile::ReadOnly))
        {
            QTextStream styleIn(&data);
            style = styleIn.readAll();
            data.close();
            //qApp->setStyleSheet(style);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace guiQt
