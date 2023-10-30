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

static const std::string COMPOSITOR_RESOURCEGROUP_NAME = "compositorsPostFX";

//------------------------------------------------------------------------------

compositor_selector::compositor_selector() noexcept
{
    new_slot(INIT_COMPOSITOR_LIST_SLOT, &compositor_selector::init_compositor_list, this);
}

//------------------------------------------------------------------------------

compositor_selector::~compositor_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void compositor_selector::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    m_layers_box       = new QComboBox();
    m_compositor_chain = new QListWidget();

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layers_box);
    layout->addWidget(m_compositor_chain);

    qt_container->set_layout(layout);

    this->refresh_renderers();

    QObject::connect(m_layers_box, SIGNAL(activated(int)), this, SLOT(on_selected_layer_item(int)));
    QObject::connect(
        m_compositor_chain,
        SIGNAL(itemChanged(QListWidgetItem*)),
        this,
        SLOT(on_selected_compositor_item(QListWidgetItem*))
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

void compositor_selector::on_selected_layer_item(int _index)
{
    m_current_layer = m_layers[static_cast<std::size_t>(_index)];
    layer::sptr layer = m_current_layer.lock();

    if(layer)
    {
        // Set current context
        layer->render_service()->make_current();

        // Empty the list widget
        m_compositor_chain->clear();

        // Update the current layer
        m_layer_compositor_chain = layer->get_compositor_chain();

        // We need the ogre's viewport in order to add the compositors,
        // this is why we have to check the viewport's existence
        if(layer->get_viewport() != nullptr)
        {
            // Fill the list widget
            this->update_compositor_list();
            // Iterates through the compositors and checks the enabled ones
            this->check_enabled_compositors();
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::on_selected_compositor_item(QListWidgetItem* _compositor_item)
{
    const std::string compositor_name = _compositor_item->text().toStdString();
    const bool is_checked             = (_compositor_item->checkState() == Qt::Checked);
    layer::sptr layer                 = m_current_layer.lock();

    if(layer)
    {
        layer->render_service()->make_current();
        layer->update_compositor_state(compositor_name, is_checked);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::init_compositor_list(layer::sptr _layer)
{
    m_current_layer = m_layers[0];

    if(_layer == m_current_layer.lock())
    {
        on_selected_layer_item(0);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::refresh_renderers()
{
    m_layers_box->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->get_layers())
        {
            const std::string id  = layer_map.first;
            std::string render_id = render->get_id();
            m_layers_box->addItem(QString::fromStdString(render_id + " : " + id));
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
        this->on_selected_layer_item(0);
    }
}

//------------------------------------------------------------------------------

void compositor_selector::update_compositor_list()
{
    layer::sptr layer = m_current_layer.lock();

    if(layer)
    {
        layer->render_service()->make_current();

        // Retrieving all compositors
        Ogre::ResourceManager::ResourceMapIterator iter =
            Ogre::CompositorManager::getSingleton().getResourceIterator();
        while(iter.hasMoreElements())
        {
            Ogre::ResourcePtr compositor = iter.getNext();
            if(compositor->getGroup() == COMPOSITOR_RESOURCEGROUP_NAME)
            {
                QString compositor_name = compositor.get()->getName().c_str();
                layer->add_available_compositor(compositor_name.toStdString());

                auto* new_compositor = new QListWidgetItem(compositor_name, m_compositor_chain);
                new_compositor->setFlags(new_compositor->flags() | Qt::ItemIsUserCheckable);
                new_compositor->setCheckState(Qt::Unchecked);
            }
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::check_enabled_compositors()
{
    layer::sptr layer = m_current_layer.lock();

    if(layer)
    {
        layer->render_service()->make_current();

        if(!m_layer_compositor_chain.empty())
        {
            for(int i(0) ; i < m_compositor_chain->count() ; ++i)
            {
                QListWidgetItem* current_compositor = m_compositor_chain->item(i);
                std::string current_compositor_name = current_compositor->text().toStdString();

                auto layer_compositor = std::find_if(
                    m_layer_compositor_chain.begin(),
                    m_layer_compositor_chain.end(),
                    [&current_compositor_name](const auto& _compositor)
                    {
                        return _compositor.first == current_compositor_name;
                    });

                if(layer_compositor != m_layer_compositor_chain.end())
                {
                    if(layer_compositor->second)
                    {
                        current_compositor->setCheckState(Qt::Checked);
                        layer->update_compositor_state(current_compositor->text().toStdString(), true);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void compositor_selector::uncheck_compositors()
{
    for(int i(0) ; i < m_compositor_chain->count() ; ++i)
    {
        QListWidgetItem* current_compositor = m_compositor_chain->item(i);
        current_compositor->setCheckState(Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

bool compositor_selector::is_enabled_compositor(const std::string& _compositor_name)
{
    auto layer_compositor = std::find_if(
        m_layer_compositor_chain.begin(),
        m_layer_compositor_chain.end(),
        [&_compositor_name](const auto& _compositor)
        {
            return _compositor.first == _compositor_name;
        });

    if(layer_compositor != m_layer_compositor_chain.end())
    {
        return layer_compositor->second;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
