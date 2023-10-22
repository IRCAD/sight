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

#include "stereo_selector.hpp"

#include <core/com/slots.hxx>

#include <data/composite.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/render.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QHBoxLayout>
#include <QWidget>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------

stereo_selector::stereo_selector() noexcept =
    default;

//------------------------------------------------------------------------------

stereo_selector::~stereo_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void stereo_selector::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    m_layersBox = new QComboBox();
    m_modeBox   = new QComboBox();

    auto* layout = new QHBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_modeBox);

    qt_container->setLayout(layout);

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

void stereo_selector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void stereo_selector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void stereo_selector::updating()
{
}

//--------------------------------------------------------------------------C---

void stereo_selector::onSelectedLayerItem(int _index)
{
    // Update the current layer
    m_currentLayer = m_layers[static_cast<std::size_t>(_index)];
}

//------------------------------------------------------------------------------

void stereo_selector::onSelectedModeItem(int _index)
{
    using sight::viz::scene3d::compositor::core;
    m_currentLayer.lock()->setStereoMode(
        _index == 1 ? core::stereo_mode_t::AUTOSTEREO_5
                    : _index == 2 ? core::stereo_mode_t::AUTOSTEREO_8
                                  : core::stereo_mode_t::NONE
    );
}

//------------------------------------------------------------------------------

void stereo_selector::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->getLayers())
        {
            const std::string& id = layer_map.first;
            if(id != sight::viz::scene3d::render::s_OGREBACKGROUNDID)
            {
                m_layersBox->addItem(QString::fromStdString(id));
                m_layers.push_back(layer_map.second);
            }
        }
    }

    if(!m_layers.empty())
    {
        m_currentLayer = m_layers[0];
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
