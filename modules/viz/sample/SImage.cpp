/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/sample/SImage.hpp"

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

namespace sight::module::viz::sample
{

static const std::string s_IMAGE_INPUT = "image";

//------------------------------------------------------------------------------

SImage::SImage() noexcept =
    default;

//------------------------------------------------------------------------------

SImage::~SImage() noexcept =
    default;

//------------------------------------------------------------------------------

void SImage::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void SImage::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    const auto genericSceneId = this->get_id() + "-genericScene";
    sight::ui::registry::registerSIDContainer(genericSceneId, qtContainer);

    // create and register the render service
    service::config_t renderConfig;
    renderConfig.put("scene.background.<xmlattr>.color", "#36393E");
    renderConfig.put("scene.layer.<xmlattr>.id", "default");
    renderConfig.put("scene.layer.<xmlattr>.order", "1");
    renderConfig.put("scene.layer.<xmlattr>.transparency", "");

    service::config_t interactorCfg;
    interactorCfg.put("<xmlattr>.uid", this->get_id() + "interactorAdaptor");
    service::config_t negatoCfg;
    negatoCfg.put("<xmlattr>.uid", this->get_id() + "negato3DAdaptor");

    renderConfig.add_child("scene.layer.adaptor", interactorCfg);
    renderConfig.add_child("scene.layer.adaptor", negatoCfg);

    m_renderSrv = sight::service::add("sight::viz::scene3d::SRender");
    m_renderSrv->setConfiguration(renderConfig);
    m_renderSrv->set_id(genericSceneId);

    m_renderSrv->configure();

    m_interactorSrv = sight::service::add("sight::module::viz::scene3d::adaptor::STrackballCamera");
    m_interactorSrv->set_id(this->get_id() + "interactorAdaptor");
    m_interactorSrv->configure();

    // Create default transfer function
    m_tf = data::TransferFunction::createDefaultTF();

    auto image = m_image.lock();
    service::config_t negatoConfig;
    negatoConfig.put("config.<xmlattr>.interactive", "true");
    m_negatoSrv = sight::service::add("sight::module::viz::scene3d::adaptor::SNegato3D");
    m_negatoSrv->setConfiguration(negatoConfig);
    m_negatoSrv->setInput(image.get_shared(), "image", true);
    m_negatoSrv->setInOut(m_tf, "tf", true);
    m_negatoSrv->set_id(this->get_id() + "negato3DAdaptor");
    m_negatoSrv->configure();

    m_renderSrv->start().wait();
    m_interactorSrv->start().wait();
    m_negatoSrv->start().wait();
}

//------------------------------------------------------------------------------

service::connections_t SImage::getAutoConnections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    connections_t connections;
    connections.push(s_IMAGE_INPUT, data::Object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void SImage::updating()
{
}

//------------------------------------------------------------------------------

void SImage::stopping()
{
    m_negatoSrv->stop().wait();
    m_interactorSrv->stop().wait();
    m_renderSrv->stop().wait();

    sight::ui::registry::unregisterSIDContainer(this->get_id() + "-genericScene");

    sight::service::remove(m_negatoSrv);
    sight::service::remove(m_interactorSrv);
    sight::service::remove(m_renderSrv);

    m_negatoSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();
    m_tf.reset();

    this->destroy();
}

} // namespace sight::module::viz::sample.
