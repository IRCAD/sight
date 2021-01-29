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

#include "Tuto01DataServiceBasicCpp/Plugin.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <services/op/Add.hpp>
#include <services/registry/ObjectService.hpp>

namespace Tuto01DataServiceBasicCpp
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto01DataServiceBasicCpp::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    // Objects declaration.
    m_image = data::Image::New();

    // UI declaration
    m_frameSrv = services::add("::sight::modules::ui::base::frame::SDefaultFrame");
    {
        services::IService::ConfigType config;
        config.put("gui.frame.name", "Tuto01DataServiceBasicCpp");
        config.put("gui.frame.icon", "Tuto01DataServiceBasicCpp-0.2/tuto.ico");
        config.put("gui.frame.minSize.<xmlattr>.width", "800");
        config.put("gui.frame.minSize.<xmlattr>.height", "600");
        config.put("registry.view.<xmlattr>.sid", "imageRendereSrv");
        m_frameSrv->setConfiguration(config);
        m_frameSrv->configure();
    }

    // Services
    m_readerSrv = services::add("::modules::io::vtk::SImageReader");
    {
        m_readerSrv->registerInOut(m_image, "data");
        services::IService::ConfigType config;
        config.put("file", "../../data/patient1.vtk");
        m_readerSrv->setConfiguration(config);
        m_readerSrv->configure();
    }

    m_renderSrv = services::add("::visuBasic::SImage");
    {
        m_renderSrv->registerInput(m_image, "image");
        m_renderSrv->setID("imageRendereSrv");
        m_renderSrv->configure();
    }

    // Start services.
    m_frameSrv->start();
    m_readerSrv->start();
    m_renderSrv->start();

    // Update services.
    m_readerSrv->update();
    m_renderSrv->update();
}

//--------------------------------------renderConfig----------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize()
{
    // stop the services
    m_renderSrv->stop();
    m_readerSrv->stop();
    m_frameSrv->stop();

    // unregister the services
    services::OSR::unregisterService( m_readerSrv );
    services::OSR::unregisterService( m_frameSrv );
    services::OSR::unregisterService( m_renderSrv );
    m_image.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto01DataServiceBasicCpp.
