/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/slots.hxx>

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/SRender.hpp>

#include <modules/ui/viz/helper/ParameterEditor.hpp>

#include <QWidget>

namespace sight::module::ui::viz
{

const core::com::slots::key_t SCompositorParameterEditor::UPDATE_COMPOSITOR_SLOT = "updateCompositor";

//------------------------------------------------------------------------------
SCompositorParameterEditor::SCompositorParameterEditor() noexcept
{
    new_slot(UPDATE_COMPOSITOR_SLOT, &SCompositorParameterEditor::updateCompositor, this);
}

//------------------------------------------------------------------------------

SCompositorParameterEditor::~SCompositorParameterEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SCompositorParameterEditor::configuring()
{
    this->initialize();

    auto config = this->getConfiguration();

    m_layerID = config.get<std::string>("layer.<xmlattr>.id", "");
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
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

void SCompositorParameterEditor::updateCompositor(
    std::string /*_compositorName*/,
    bool /*_enabled*/,
    sight::viz::scene3d::Layer::sptr _layer
)
{
    if(_layer->getLayerID() == m_layerID)
    {
        // We will create a new layout so clear everything before
        this->clear();

        bool found = false;

        const auto adaptors = _layer->getRegisteredAdaptors();

        // Is there at least one parameter that we can handle ?
        for(const auto& wAdaptor : adaptors)
        {
            const auto adaptor = wAdaptor.lock();
            if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::SCompositorParameter")
            {
                /// Filter object types
                const auto shaderObj =
                    adaptor->inout(sight::viz::scene3d::IParameter::s_PARAMETER_INOUT).lock();
                const auto& objType = shaderObj->get_classname();

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
        for(const auto& wAdaptor : adaptors)
        {
            const auto adaptor = wAdaptor.lock();
            if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::SCompositorParameter")
            {
                auto paramAdaptor = std::dynamic_pointer_cast<const sight::viz::scene3d::IParameter>(adaptor);
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

        this->fillGui();
    }
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::clear()
{
    service::base::sptr objService = m_editorInfo.srv.lock();

    if(objService)
    {
        objService->stop();

        sight::ui::registry::unregisterSIDContainer(m_editorInfo.uuid);

        sight::service::unregisterService(objService);

        m_sizer->removeWidget(m_editorInfo.editorPanel->getQtContainer());
        m_editorInfo.editorPanel->destroyContainer();
        m_editorInfo.editorPanel.reset();
    }
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::fillGui()
{
    auto editorService = m_editorInfo.srv.lock();
    if(editorService)
    {
        m_sizer->addWidget(m_editorInfo.editorPanel->getQtContainer(), 0);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
