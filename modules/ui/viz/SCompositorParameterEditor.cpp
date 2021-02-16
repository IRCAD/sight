/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "SCompositorParameterEditor.hpp"

#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/SRender.hpp>

#include <modules/ui/viz/helper/ParameterEditor.hpp>

#include <QWidget>

#include <ui/base/GuiRegistry.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::modules::ui::viz
{

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::modules::ui::viz::SCompositorParameterEditor)

const core::com::Slots::SlotKeyType SCompositorParameterEditor::s_UPDATE_COMPOSITOR_SLOT = "updateCompositor";

//------------------------------------------------------------------------------
SCompositorParameterEditor::SCompositorParameterEditor() noexcept
{
    newSlot(s_UPDATE_COMPOSITOR_SLOT, &SCompositorParameterEditor::updateCompositor, this);
}

//------------------------------------------------------------------------------

SCompositorParameterEditor::~SCompositorParameterEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::configuring()
{
    this->initialize();

    auto config = this->getConfigTree();

    m_layerID = config.get<std::string>("layer.<xmlattr>.id", "");
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer() );
    m_sizer = new QVBoxLayout();
    m_sizer->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::stopping()
{
    this->clear();
    this->destroy();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::updating()
{
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::updateCompositor(std::string /*_compositorName*/, bool /*_enabled*/,
                                                  sight::viz::ogre::Layer::sptr _layer)
{
    if(_layer->getLayerID() == m_layerID)
    {
        // We will create a new layout so clear everything before
        this->clear();

        bool found = false;

        const auto adaptors = _layer->getRegisteredAdaptors();

        // Is there at least one parameter that we can handle ?
        for (const auto& wAdaptor : adaptors)
        {
            const auto adaptor = wAdaptor.lock();
            if (adaptor->getClassname() == "::modules::viz::ogre::adaptor::SCompositorParameter")
            {
                /// Filter object types
                const data::Object::csptr shaderObj =
                    adaptor->getInOut< data::Object>(sight::viz::ogre::IParameter::s_PARAMETER_INOUT);
                const auto& objType = shaderObj->getClassname();

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

        auto editorService = service::add( "::modules::ui::qt::editor::SParameters", m_editorInfo.uuid );
        m_editorInfo.service = editorService;

        service::IService::ConfigType editorConfig;

        // Get all ShaderParameter subservices from the corresponding Material adaptor
        for (const auto& wAdaptor : adaptors)
        {
            const auto adaptor = wAdaptor.lock();
            if (adaptor->getClassname() == "::modules::viz::ogre::adaptor::SCompositorParameter")
            {
                auto paramAdaptor = sight::viz::ogre::IParameter::dynamicConstCast(adaptor);
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

        this->fillGui();
    }
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::clear()
{
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

void SCompositorParameterEditor::fillGui()
{
    auto editorService = m_editorInfo.service.lock();
    if(editorService)
    {
        m_sizer->addWidget( m_editorInfo.editorPanel->getQtContainer(), 0 );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::modules::ui::viz
