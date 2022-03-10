/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "Plugin.hpp"

#include <core/runtime/operations.hpp>

#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

using namespace sight;

namespace Tuto01DataServiceBasicCpp
{

SIGHT_REGISTER_PLUGIN("Tuto01DataServiceBasicCpp::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    // Objects declaration.
    m_image = data::Image::New();

    // UI declaration
    m_frameSrv = service::add("sight::module::ui::base::SFrame");
    {
        service::IService::ConfigType config;
        config.put("gui.frame.name", "Tuto01DataServiceBasicCpp");
        config.put("gui.frame.icon", "Tuto01DataServiceBasicCpp/tuto.ico");
        config.put("gui.frame.minSize.<xmlattr>.width", "800");
        config.put("gui.frame.minSize.<xmlattr>.height", "600");
        config.put("registry.view.<xmlattr>.sid", "imageRendereSrv");
        m_frameSrv->setConfiguration(config);
        m_frameSrv->configure();
    }

    // Services
    m_readerSrv = service::add("sight::module::io::vtk::SImageReader");
    {
        m_readerSrv->setInOut(m_image, "data");
        service::IService::ConfigType config;
        config.put("resource", "Tuto02DataServiceBasic/img-test.vtk");
        m_readerSrv->setConfiguration(config);
        m_readerSrv->configure();
    }

    m_renderSrv = service::add("sight::module::viz::sample::SImage");
    {
        m_renderSrv->setInput(m_image, "image");
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

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    // stop the services
    m_renderSrv->stop();
    m_readerSrv->stop();
    m_frameSrv->stop();

    // unregister the services
    service::OSR::unregisterService(m_readerSrv);
    service::OSR::unregisterService(m_frameSrv);
    service::OSR::unregisterService(m_renderSrv);

    m_readerSrv.reset();
    m_frameSrv.reset();
    m_renderSrv.reset();

    m_image.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto01DataServiceBasicCpp.
