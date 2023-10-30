/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "viz/qt3d/core/generic_scene.hpp"

#include "viz/qt3d/core/frame_graph.hpp"
#include "viz/qt3d/data/mesh.hpp"

#include <QClearBuffers>
#include <QPhongMaterial>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QOrbitCameraController>

namespace sight::viz::qt3d::core
{

generic_scene::generic_scene(bool _is_qml, Qt3DCore::QNode* _parent) :
    QEntity(_parent),
    m_camera(new Qt3DRender::QCamera(this)),
    m_camera_controller(new Qt3DExtras::QOrbitCameraController(this)),
    m_frame_graph(new frame_graph())
{
    // Defines the camera that will be used to render the scene.
    m_camera->lens()->setPerspectiveProjection(45.0F, 16.0F / 9.0F, 0.1F, 10000.0F);
    m_camera->setPosition(QVector3D(0.F, 0.F, 0.F));
    m_camera->setViewCenter(QVector3D(0.F, 0.F, 0.F));

    // Defines a camera controller allowing user to control camera with the mouse
    // and sets it as a child of the scene entity.
    m_camera_controller->setCamera(m_camera);
    m_camera_controller->setLinearSpeed(500.0F);
    m_camera_controller->setLookSpeed(480.0F);

    // Defines a default framegraph.
    m_frame_graph->set_camera(m_camera);
    m_frame_graph->set_buffers_to_clear(Qt3DRender::QClearBuffers::AllBuffers);

    if(_is_qml)
    {
        m_input_settings = new Qt3DInput::QInputSettings(this);
        this->addComponent(m_input_settings);

        // Defines render settings allowing to define custom framegraphs.
        // Sets previously defined ForwardRenderer as default renderer.
        m_render_settings = new Qt3DRender::QRenderSettings(this);
        m_render_settings->setActiveFrameGraph(m_frame_graph);
        this->addComponent(m_render_settings);
    }
}

//------------------------------------------------------------------------------

generic_scene::~generic_scene()
= default;

//------------------------------------------------------------------------------

Qt3DRender::QCamera* generic_scene::get_camera()
{
    return m_camera;
}

//------------------------------------------------------------------------------

Qt3DExtras::QAbstractCameraController* generic_scene::get_camera_controller()
{
    return m_camera_controller;
}

//------------------------------------------------------------------------------

Qt3DInput::QInputSettings* generic_scene::get_input_settings()
{
    return m_input_settings;
}

//------------------------------------------------------------------------------

Qt3DRender::QRenderSettings* generic_scene::get_render_settings()
{
    return m_render_settings;
}

//------------------------------------------------------------------------------

frame_graph* generic_scene::get_frame_graph()
{
    return m_frame_graph;
}

//------------------------------------------------------------------------------

void generic_scene::set_camera(Qt3DRender::QCamera* _camera)
{
    SIGHT_ASSERT("Camera can't be set to null value.", _camera);
    m_camera = _camera;
    m_frame_graph->set_camera(_camera);
}

//------------------------------------------------------------------------------

void generic_scene::set_camera_controller(Qt3DExtras::QAbstractCameraController* _camera_controller)
{
    m_camera_controller = _camera_controller;
}

//------------------------------------------------------------------------------

void generic_scene::set_input_settings(Qt3DInput::QInputSettings* _input_settings)
{
    m_input_settings = _input_settings;
}

//------------------------------------------------------------------------------

void generic_scene::set_render_settings(Qt3DRender::QRenderSettings* _render_settings)
{
    m_render_settings = _render_settings;
}

//------------------------------------------------------------------------------

void generic_scene::set_frame_graph(frame_graph* _frame_graph)
{
    SIGHT_ASSERT("FrameGraph can't be set to null value.", _frame_graph);
    m_frame_graph = _frame_graph;
}

} // namespace sight::viz::qt3d::core
