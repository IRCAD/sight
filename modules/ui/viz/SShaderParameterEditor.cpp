/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "SShaderParameterEditor.hpp"

#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <service/op.hpp>

#include <ui/__/registry.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <modules/ui/viz/helper/ParameterEditor.hpp>

#include <QWidget>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------
SShaderParameterEditor::SShaderParameterEditor() noexcept =
    default;

//------------------------------------------------------------------------------

SShaderParameterEditor::~SShaderParameterEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SShaderParameterEditor::starting()
{
    {
        const auto rec                = m_reconstruction.lock();
        data::Material::sptr material = rec->getMaterial();
        m_connections.connect(material, data::Material::MODIFIED_SIG, this->get_sptr(), service::slots::UPDATE);
    }

    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_sizer = new QVBoxLayout();
    m_sizer->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::stopping()
{
    m_connections.disconnect();
    this->clear();
    this->destroy();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updating()
{
    this->clear();
    this->updateGuiInfo();
    this->fillGui();
}

//------------------------------------------------------------------------------
void SShaderParameterEditor::clear()
{
    m_editorInfo.connections.disconnect();

    service::base::sptr objService = m_editorInfo.srv.lock();

    if(objService)
    {
        objService->stop();

        sight::ui::registry::unregisterSIDContainer(m_editorInfo.uuid);

        sight::service::remove(objService);

        m_sizer->removeWidget(m_editorInfo.editorPanel->getQtContainer());
        m_editorInfo.editorPanel->destroyContainer();
        m_editorInfo.editorPanel.reset();
    }
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updateGuiInfo()
{
    /// Getting all Material adaptors
    const auto reconstruction = m_reconstruction.lock();

    const auto srvVec = sight::service::getServices("sight::module::viz::scene3d::adaptor::SMaterial");

    /// Stop if no Material adaptors have been find
    if(srvVec.empty())
    {
        SIGHT_WARN("No module::viz::scene3d::adaptor::SMaterial found in the application");
        return;
    }

    /// Try to find the material adaptor working with the same data::Material
    /// as the one contained by the current reconstruction
    sight::viz::scene3d::adaptor::sptr matService;
    for(const auto& srv : srvVec)
    {
        if(srv->inout("material").lock()->get_id() == reconstruction->getMaterial()->get_id())
        {
            matService = std::dynamic_pointer_cast<sight::viz::scene3d::adaptor>(srv);
            break;
        }
    }

    SIGHT_ASSERT("Material adaptor corresponding to the current Reconstruction must exist", matService);

    bool found = false;

    // Is there at least one parameter that we can handle ?
    for(const auto& wParamSrv : matService->getRegisteredServices())
    {
        const auto paramSrv = wParamSrv.lock();
        if(paramSrv->get_classname() == "sight::module::viz::scene3d::adaptor::SShaderParameter")
        {
            /// Filter object types
            const auto shaderObj =
                paramSrv->inout(sight::viz::scene3d::IParameter::s_PARAMETER_INOUT).lock();
            const ObjectClassnameType objType = shaderObj->get_classname();

            if(objType == "sight::data::Boolean" || objType == "sight::data::Float"
               || objType == "sight::data::Integer")
            {
                found = true;
                break;
            }
        }
    }

    if(!found)
    {
        return;
    }

    /// Getting this widget's container
    auto qtContainer   = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();

    auto* p2 = new QWidget(container);
    m_editorInfo.editorPanel = sight::ui::qt::container::widget::make();
    m_editorInfo.editorPanel->setQtContainer(p2);

    const std::string uuid = this->get_id();
    m_editorInfo.uuid = uuid + "-editor";

    sight::ui::registry::registerSIDContainer(m_editorInfo.uuid, m_editorInfo.editorPanel);

    auto editorService = sight::service::add("sight::module::ui::qt::SParameters", m_editorInfo.uuid);
    m_editorInfo.srv = editorService;

    service::config_t editorConfig;

    // Get all ShaderParameter subservices from the corresponding Material adaptor
    for(const auto& wAdaptor : matService->getRegisteredServices())
    {
        const auto adaptor = wAdaptor.lock();
        if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::SShaderParameter")
        {
            auto paramAdaptor = std::dynamic_pointer_cast<sight::viz::scene3d::IParameter>(adaptor);
            auto paramConfig  = module::ui::viz::helper::ParameterEditor::createConfig(
                paramAdaptor,
                m_editorInfo.srv.lock(),
                m_editorInfo.connections
            );

            if(!paramConfig.empty())
            {
                editorConfig.add_child("parameters.param", paramConfig);
            }
        }
    }

    editorService->setConfiguration(editorConfig);
    editorService->configure();

    editorService->start();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::fillGui()
{
    auto editorService = m_editorInfo.srv.lock();
    if(editorService)
    {
        m_sizer->addWidget(m_editorInfo.editorPanel->getQtContainer(), 0);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
