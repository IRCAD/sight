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

#include "uiVisuOgre/SStereoSelector.hpp"

#include <core/com/Slots.hxx>

#include <data/Composite.hpp>

#include <services/macros.hpp>
#include <services/registry/ObjectService.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QHBoxLayout>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>

#include <viz/ogre/SRender.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::uiVisuOgre::SStereoSelector, ::sight::data::Composite)

//------------------------------------------------------------------------------

SStereoSelector::SStereoSelector() noexcept
{
}

//------------------------------------------------------------------------------

SStereoSelector::~SStereoSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SStereoSelector::starting()
{
    this->create();

    ui::qt::container::QtContainer::sptr qtContainer = ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_layersBox = new QComboBox();
    m_modeBox   = new QComboBox();

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_modeBox);

    qtContainer->setLayout( layout );

    this->refreshRenderers();

    m_modeBox->addItem("Mono");
    m_modeBox->addItem("Stereo 5 views");
    m_modeBox->addItem("Stereo 8 views");

    auto layer = m_currentLayer.lock();
    if(layer)
    {
        m_modeBox->setCurrentIndex(static_cast<int>(m_currentLayer.lock()->getStereoMode()));
    }

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect(m_modeBox, SIGNAL(activated(int)), this, SLOT(onSelectedModeItem(int)));
}

//------------------------------------------------------------------------------

void SStereoSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SStereoSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SStereoSelector::updating()
{
}

//------------------------------------------------------------------------------

void SStereoSelector::onSelectedLayerItem(int index)
{
    // Update the current layer
    m_currentLayer = m_layers[static_cast<size_t>(index)];
}

//------------------------------------------------------------------------------

void SStereoSelector::onSelectedModeItem(int index)
{
    m_currentLayer.lock()->setStereoMode(index == 1 ? viz::ogre::compositorcore::StereoModeType::AUTOSTEREO_5 :
                                         index == 2 ? viz::ogre::compositorcore::StereoModeType::AUTOSTEREO_8 :
                                         viz::ogre::compositorcore::StereoModeType::NONE);
}

//------------------------------------------------------------------------------

void SStereoSelector::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    services::registry::ObjectService::ServiceVectorType renderers =
        services::OSR::getServices("::sight::viz::ogre::SRender");

    for(auto srv : renderers)
    {
        viz::ogre::SRender::sptr render = viz::ogre::SRender::dynamicCast(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string& id = layerMap.first;
            if(id != viz::ogre::SRender::s_OGREBACKGROUNDID)
            {
                m_layersBox->addItem(QString::fromStdString(id));
                m_layers.push_back(layerMap.second);
            }
        }
    }

    if(!m_layers.empty())
    {
        m_currentLayer = m_layers[0];
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
