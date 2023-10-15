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

#include "compositor_selector.hpp"

#include <core/com/slots.hxx>

#include <data/composite.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/render.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QListWidgetItem>
#include <QWidget>

namespace sight::module::ui::viz
{

using sight::viz::scene3d::Layer;

const core::com::slots::key_t compositor_selector::INIT_COMPOSITOR_LIST_SLOT = "initCompositorList";

static const std::string s_COMPOSITOR_RESOURCEGROUP_NAME = "compositorsPostFX";

//------------------------------------------------------------------------------

compositor_selector::compositor_selector() noexcept
{
    new_slot(INIT_COMPOSITOR_LIST_SLOT, &compositor_selector::initCompositorList, this);
}

//------------------------------------------------------------------------------

compositor_selector::~compositor_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void compositor_selector::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    m_layersBox       = new QComboBox();
    m_compositorChain = new QListWidget();

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_compositorChain);

    qtContainer->setLayout(layout);

    this->refreshRenderers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect(
        m_compositorChain,
        SIGNAL(itemChanged(QListWidgetItem*)),
        this,
        SLOT(onSelectedCompositorItem(QListWidgetItem*))
    );
}

//------------------------------------------------------------------------------

void compositor_selector::stopping()
{
    m_connections.disconnect();

    this->destroy();
}

//------------------------------------------------------------------------------

void compositor_selector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void compositor_selector::updating()
{
}

//------------------------------------------------------------------------------

void compositor_selector::onSelectedLayerItem(int index)
{
    m_currentLayer = m_layers[static_cast<std::size_t>(index)];
    Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        // Set current context
        layer->getRenderService()->makeCurrent();

        // Empty the list widget
        m_compositorChain->clear();

        // Update the current layer
        m_layerCompositorChain = layer->getCompositorChain();

        // We need the ogre's viewport in order to add the compositors,
        // this is why we have to check the viewport's existence
        if(layer->getViewport() != nullptr)
        {
            // Fill the list widget
            this->updateCompositorList();
            // Iterates through the compositors and checks the enabled ones
            this->checkEnabledCompositors();
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::onSelectedCompositorItem(QListWidgetItem* compositorItem)
{
    const std::string compositorName = compositorItem->text().toStdString();
    const bool isChecked             = (compositorItem->checkState() == Qt::Checked);
    Layer::sptr layer                = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();
        layer->updateCompositorState(compositorName, isChecked);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::initCompositorList(Layer::sptr layer)
{
    m_currentLayer = m_layers[0];

    if(layer == m_currentLayer.lock())
    {
        onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::getServices("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string id = layerMap.first;
            std::string renderID = render->get_id();
            m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
            m_layers.push_back(layerMap.second);

            m_connections.connect(
                layerMap.second,
                Layer::INIT_LAYER_SIG,
                this->get_sptr(),
                INIT_COMPOSITOR_LIST_SLOT
            );
        }
    }

    if(!m_layers.empty())
    {
        this->onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::updateCompositorList()
{
    Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();

        // Retrieving all compositors
        Ogre::ResourceManager::ResourceMapIterator iter =
            Ogre::CompositorManager::getSingleton().getResourceIterator();
        while(iter.hasMoreElements())
        {
            Ogre::ResourcePtr compositor = iter.getNext();
            if(compositor->getGroup() == s_COMPOSITOR_RESOURCEGROUP_NAME)
            {
                QString compositorName = compositor.get()->getName().c_str();
                layer->addAvailableCompositor(compositorName.toStdString());

                auto* newCompositor = new QListWidgetItem(compositorName, m_compositorChain);
                newCompositor->setFlags(newCompositor->flags() | Qt::ItemIsUserCheckable);
                newCompositor->setCheckState(Qt::Unchecked);
            }
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::checkEnabledCompositors()
{
    Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();

        if(!m_layerCompositorChain.empty())
        {
            for(int i(0) ; i < m_compositorChain->count() ; ++i)
            {
                QListWidgetItem* currentCompositor = m_compositorChain->item(i);
                std::string currentCompositorName  = currentCompositor->text().toStdString();

                auto layerCompositor = std::find_if(
                    m_layerCompositorChain.begin(),
                    m_layerCompositorChain.end(),
                    [&currentCompositorName](const auto& _compositor)
                    {
                        return _compositor.first == currentCompositorName;
                    });

                if(layerCompositor != m_layerCompositorChain.end())
                {
                    if(layerCompositor->second)
                    {
                        currentCompositor->setCheckState(Qt::Checked);
                        layer->updateCompositorState(currentCompositor->text().toStdString(), true);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::uncheckCompositors()
{
    for(int i(0) ; i < m_compositorChain->count() ; ++i)
    {
        QListWidgetItem* currentCompositor = m_compositorChain->item(i);
        currentCompositor->setCheckState(Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

bool compositor_selector::isEnabledCompositor(const std::string& compositorName)
{
    auto layerCompositor = std::find_if(
        m_layerCompositorChain.begin(),
        m_layerCompositorChain.end(),
        [&compositorName](const auto& _compositor)
        {
            return _compositor.first == compositorName;
        });

    if(layerCompositor != m_layerCompositorChain.end())
    {
        return layerCompositor->second;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
