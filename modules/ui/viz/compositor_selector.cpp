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

using sight::viz::scene3d::layer;

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

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    m_layersBox       = new QComboBox();
    m_compositorChain = new QListWidget();

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_compositorChain);

    qt_container->setLayout(layout);

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

void compositor_selector::onSelectedLayerItem(int _index)
{
    m_currentLayer = m_layers[static_cast<std::size_t>(_index)];
    layer::sptr layer = m_currentLayer.lock();

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

void compositor_selector::onSelectedCompositorItem(QListWidgetItem* _compositor_item)
{
    const std::string compositor_name = _compositor_item->text().toStdString();
    const bool is_checked             = (_compositor_item->checkState() == Qt::Checked);
    layer::sptr layer                 = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();
        layer->updateCompositorState(compositor_name, is_checked);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::initCompositorList(layer::sptr _layer)
{
    m_currentLayer = m_layers[0];

    if(_layer == m_currentLayer.lock())
    {
        onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->getLayers())
        {
            const std::string id  = layer_map.first;
            std::string render_id = render->get_id();
            m_layersBox->addItem(QString::fromStdString(render_id + " : " + id));
            m_layers.push_back(layer_map.second);

            m_connections.connect(
                layer_map.second,
                layer::INIT_LAYER_SIG,
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
    layer::sptr layer = m_currentLayer.lock();

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
                QString compositor_name = compositor.get()->getName().c_str();
                layer->addAvailableCompositor(compositor_name.toStdString());

                auto* new_compositor = new QListWidgetItem(compositor_name, m_compositorChain);
                new_compositor->setFlags(new_compositor->flags() | Qt::ItemIsUserCheckable);
                new_compositor->setCheckState(Qt::Unchecked);
            }
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::checkEnabledCompositors()
{
    layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();

        if(!m_layerCompositorChain.empty())
        {
            for(int i(0) ; i < m_compositorChain->count() ; ++i)
            {
                QListWidgetItem* current_compositor = m_compositorChain->item(i);
                std::string current_compositor_name = current_compositor->text().toStdString();

                auto layer_compositor = std::find_if(
                    m_layerCompositorChain.begin(),
                    m_layerCompositorChain.end(),
                    [&current_compositor_name](const auto& _compositor)
                    {
                        return _compositor.first == current_compositor_name;
                    });

                if(layer_compositor != m_layerCompositorChain.end())
                {
                    if(layer_compositor->second)
                    {
                        current_compositor->setCheckState(Qt::Checked);
                        layer->updateCompositorState(current_compositor->text().toStdString(), true);
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
        QListWidgetItem* current_compositor = m_compositorChain->item(i);
        current_compositor->setCheckState(Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

bool compositor_selector::isEnabledCompositor(const std::string& _compositor_name)
{
    auto layer_compositor = std::find_if(
        m_layerCompositorChain.begin(),
        m_layerCompositorChain.end(),
        [&_compositor_name](const auto& _compositor)
        {
            return _compositor.first == _compositor_name;
        });

    if(layer_compositor != m_layerCompositorChain.end())
    {
        return layer_compositor->second;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
