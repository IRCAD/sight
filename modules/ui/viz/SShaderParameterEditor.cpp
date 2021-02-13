/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <viz/ogre/IAdaptor.hpp>

#include <modules/ui/viz/helper/ParameterEditor.hpp>

#include <QWidget>

#include <ui/base/GuiRegistry.hpp>

namespace sight::modules::ui::viz
{

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::sight::modules::ui::viz::SShaderParameterEditor,
                         ::sight::data::Reconstruction)

static const std::string s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------
SShaderParameterEditor::SShaderParameterEditor() noexcept
{
}

//------------------------------------------------------------------------------

SShaderParameterEditor::~SShaderParameterEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::starting()
{
    data::Reconstruction::sptr rec = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    data::Material::sptr material  = rec->getMaterial();
    m_connections.connect(material, data::Material::s_MODIFIED_SIG, this->getSptr(), s_UPDATE_SLOT);

    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast( this->getContainer() );

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

void SShaderParameterEditor::swapping()
{
    m_connections.disconnect();
    data::Reconstruction::sptr rec = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    data::Material::sptr material  = rec->getMaterial();

    m_connections.connect(material, data::Material::s_MODIFIED_SIG, this->getSptr(), s_UPDATE_SLOT);

    this->updating();
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

    service::IService::sptr objService = m_editorInfo.service.lock();

    if(objService)
    {
        objService->stop();

        ::sight::ui::base::GuiRegistry::unregisterSIDContainer(m_editorInfo.uuid);

        service::OSR::unregisterService(objService);

        m_sizer->removeWidget(m_editorInfo.editorPanel->getQtContainer());
        m_editorInfo.editorPanel->destroyContainer();
        m_editorInfo.editorPanel.reset();
    }
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updateGuiInfo()
{
    /// Getting all Material adaptors
    auto reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);

    service::registry::ObjectService::ServiceVectorType srvVec = service::OSR::getServices(
        "::modules::viz::ogre::adaptor::SMaterial");

    /// Stop if no Material adaptors have been find
    if(srvVec.empty())
    {
        SLM_WARN("No modules::viz::ogre::adaptor::SMaterial found in the application");
        return;
    }

    /// Try to find the material adaptor working with the same data::Material
    /// as the one contained by the current reconstruction
    sight::viz::ogre::IAdaptor::sptr matService;
    for (auto srv : srvVec)
    {
        if (srv->getInOut< data::Object>("material")->getID() == reconstruction->getMaterial()->getID())
        {
            matService = sight::viz::ogre::IAdaptor::dynamicCast(srv);
            break;
        }
    }

    SLM_ASSERT("Material adaptor corresponding to the current Reconstruction must exist", matService);

    bool found = false;

    // Is there at least one parameter that we can handle ?
    for (const auto& wParamSrv : matService->getRegisteredServices())
    {
        const auto paramSrv = wParamSrv.lock();
        if (paramSrv->getClassname() == "::modules::viz::ogre::adaptor::SShaderParameter")
        {
            /// Filter object types
            const data::Object::csptr shaderObj =
                paramSrv->getInOut< data::Object>(sight::viz::ogre::IParameter::s_PARAMETER_INOUT);
            const ObjectClassnameType objType = shaderObj->getClassname();

            if(objType == "::sight::data::Boolean" || objType == "::sight::data::Float" ||
               objType == "::sight::data::Integer")
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
    auto qtContainer   = sight::ui::qt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* container = qtContainer->getQtContainer();

    QWidget* p2 = new QWidget( container );
    m_editorInfo.editorPanel = sight::ui::qt::container::QtContainer::New();
    m_editorInfo.editorPanel->setQtContainer(p2);

    const std::string uuid = this->getID();
    m_editorInfo.uuid = uuid + "-editor";

    ::sight::ui::base::GuiRegistry::registerSIDContainer(m_editorInfo.uuid, m_editorInfo.editorPanel);

    auto editorService = service::add("::modules::ui::qt::editor::SParameters", m_editorInfo.uuid );
    m_editorInfo.service = editorService;

    service::IService::ConfigType editorConfig;

    // Get all ShaderParameter subservices from the corresponding Material adaptor
    for (auto wAdaptor : matService->getRegisteredServices())
    {
        const auto adaptor = wAdaptor.lock();
        if (adaptor->getClassname() == "::modules::viz::ogre::adaptor::SShaderParameter")
        {
            auto paramAdaptor = sight::viz::ogre::IParameter::dynamicCast(adaptor);
            auto paramConfig  = modules::ui::viz::helper::ParameterEditor::createConfig(paramAdaptor,
                                                                                        m_editorInfo.service.lock(),
                                                                                        m_editorInfo.connections);

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
    auto editorService = m_editorInfo.service.lock();
    if(editorService)
    {
        m_sizer->addWidget( m_editorInfo.editorPanel->getQtContainer(), 0 );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::modules::ui::viz
