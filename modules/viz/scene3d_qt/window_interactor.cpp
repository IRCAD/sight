/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d_qt/window_interactor.hpp"

#include "modules/viz/scene3d_qt/open_gl_context.hpp"
#include "modules/viz/scene3d_qt/open_gl_worker.hpp"

#include <core/com/slots.hxx>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/render.hpp>

#include <QDesktopWidget>
#include <QEvent>
#include <QGestureEvent>
#include <QGuiApplication>
#include <QRect>
#include <QShortcut>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D(
    sight::module::viz::scene3d_qt::window_interactor,
    sight::viz::scene3d::window_interactor::REGISTRY_KEY
);

namespace sight::module::viz::scene3d_qt
{

//-----------------------------------------------------------------------------

window_interactor::~window_interactor()
{
    // Delete the window container if it is not attached to the parent container.
    // i.e. it is shown in fullscreen.
    if((m_ogre_widget != nullptr) && m_ogre_widget->parent() == nullptr)
    {
        delete m_ogre_widget;
    }
}

//-----------------------------------------------------------------------------

void window_interactor::render_now()
{
    m_ogre_widget->render_now();
}

//-----------------------------------------------------------------------------

void window_interactor::request_render()
{
    m_ogre_widget->request_render();
}

//-----------------------------------------------------------------------------

void window_interactor::create_container(
    sight::ui::container::widget::sptr _parent,
    bool _fullscreen,
    const std::string& _id
)
{
    SIGHT_ASSERT("Invalid parent.", _parent);
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);

    auto* layout = new QVBoxLayout();
    m_parent_container->set_layout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_ogre_widget = new module::viz::scene3d_qt::window();

    m_ogre_widget->setObjectName(QString::fromStdString(_id));

    layout->addWidget(m_ogre_widget);
    m_ogre_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_ogre_widget->setMouseTracking(true);

    m_ogre_widget->grabGesture(Qt::PinchGesture); // For zooming
    m_ogre_widget->grabGesture(Qt::PanGesture);   // For translating

    this->set_fullscreen(_fullscreen, -1);

    const auto render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(m_render_service.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", render_service);

    std::map<int, sight::viz::scene3d::layer::wptr> ordered_layers;
    for(auto& layer : render_service->get_layers())
    {
        ordered_layers[layer.second->get_order()] = layer.second;
    }

    for(auto& layer : ordered_layers)
    {
        m_ogre_widget->register_layer(layer.second);
    }

    QShowEvent show_event;
    QCoreApplication::sendEvent(m_ogre_widget, &show_event);
}

//-----------------------------------------------------------------------------

void window_interactor::connect_to_container()
{
    // Connect widget window render to render service start adaptors
    const auto render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(m_render_service.lock());
    SIGHT_ASSERT("RenderService wrongly instantiated. ", render_service);

    QObject::connect(m_ogre_widget, &window::interacted, this, &window_interactor::on_interacted);
}

//-----------------------------------------------------------------------------

void window_interactor::disconnect_interactor()
{
    QObject::disconnect(m_ogre_widget, &window::interacted, this, &window_interactor::on_interacted);
    QWidget* const container = m_parent_container->get_qt_container();
    container->layout()->removeWidget(m_ogre_widget);
    m_ogre_widget->destroy_window();
    delete m_ogre_widget;
    m_ogre_widget = nullptr;
}

//-----------------------------------------------------------------------------

void window_interactor::make_current()
{
    m_ogre_widget->make_current();
}

//-----------------------------------------------------------------------------

int window_interactor::get_widget_id() const
{
    return m_ogre_widget->get_id();
}

//-----------------------------------------------------------------------------

int window_interactor::get_frame_id() const
{
    return m_ogre_widget->get_frame_id();
}

//-----------------------------------------------------------------------------

float window_interactor::get_logical_dots_per_inch() const
{
    SIGHT_ASSERT("Trying to query dots per inch on a non-existing windows.", m_ogre_widget);
    return static_cast<float>(m_ogre_widget->screen()->logicalDotsPerInchY());
}

//-----------------------------------------------------------------------------

void window_interactor::on_interacted(sight::viz::scene3d::window_interactor::interaction_info _info)
{
    service::base::sptr render_service                    = m_render_service.lock();
    sight::viz::scene3d::render::sptr ogre_render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(
        render_service
    );

    for(const auto& layer_map : ogre_render_service->get_layers())
    {
        sight::viz::scene3d::layer::sptr layer = layer_map.second;
        layer->slot<sight::viz::scene3d::layer::interaction_slot_t>(sight::viz::scene3d::layer::INTERACTION_SLOT)->
        async_run(
            _info
        );
    }
}

//-----------------------------------------------------------------------------

sight::viz::scene3d::graphics_worker* window_interactor::create_graphics_worker()
{
    return new open_gl_worker(m_ogre_widget);
}

//-----------------------------------------------------------------------------

void window_interactor::set_fullscreen(bool _fullscreen, int _screen_number)
{
    if(!m_fullscreen_shortcut.isNull())
    {
        m_fullscreen_shortcut->deleteLater();
        m_fullscreen_shortcut.clear();
    }

    QWidget* const container = m_parent_container->get_qt_container();
    m_is_full_screen = _fullscreen;
    if(_fullscreen)
    {
        container->layout()->removeWidget(m_ogre_widget);

        const QDesktopWidget* desktop = QApplication::desktop();

        QRect screenres;
        if(_screen_number < 0)
        {
            _screen_number = desktop->screenNumber(container) + 1;
        }

        if(_screen_number >= QGuiApplication::screens().count())
        {
            screenres = QGuiApplication::primaryScreen()->geometry();
        }
        else
        {
            screenres = QGuiApplication::screens()[_screen_number]->geometry();
        }

        m_ogre_widget->setParent(nullptr);
        m_ogre_widget->showFullScreen();
        m_ogre_widget->setGeometry(screenres);

        m_fullscreen_shortcut = new QShortcut(
            QKeySequence::FullScreen,
            m_ogre_widget,
            [this]
            {
                this->set_fullscreen(!m_is_full_screen, -1);

                auto render_service      = m_render_service.lock();
                auto ogre_render_service = std::dynamic_pointer_cast<sight::viz::scene3d::render>(render_service);

                if(m_is_full_screen)
                {
                    auto enable_full_screen_slot = ogre_render_service->slot(
                        sight::viz::scene3d::render::ENABLE_FULLSCREEN
                    );

                    enable_full_screen_slot->run(0);
                }
                else
                {
                    auto disable_full_screen_slot = ogre_render_service->slot(
                        sight::viz::scene3d::render::DISABLE_FULLSCREEN
                    );

                    disable_full_screen_slot->run();
                }
            });
    }
    else if(container->layout()->isEmpty())
    {
        container->layout()->addWidget(m_ogre_widget);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt
