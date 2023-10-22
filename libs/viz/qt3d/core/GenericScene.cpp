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

#include "viz/qt3d/core/GenericScene.hpp"

#include "viz/qt3d/core/FrameGraph.hpp"
#include "viz/qt3d/data/mesh.hpp"

#include <QClearBuffers>
#include <QPhongMaterial>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QOrbitCameraController>

namespace sight::viz::qt3d::core
{

GenericScene::GenericScene(bool _is_qml, Qt3DCore::QNode* _parent) :
    QEntity(_parent),
    m_camera(new Qt3DRender::QCamera(this)),
    m_cameraController(new Qt3DExtras::QOrbitCameraController(this)),
    m_frameGraph(new FrameGraph())
{
    // Defines the camera that will be used to render the scene.
    m_camera->lens()->setPerspectiveProjection(45.0F, 16.0F / 9.0F, 0.1F, 10000.0F);
    m_camera->setPosition(QVector3D(0.F, 0.F, 0.F));
    m_camera->setViewCenter(QVector3D(0.F, 0.F, 0.F));

    // Defines a camera controller allowing user to control camera with the mouse
    // and sets it as a child of the scene entity.
    m_cameraController->setCamera(m_camera);
    m_cameraController->setLinearSpeed(500.0F);
    m_cameraController->setLookSpeed(480.0F);

    // Defines a default framegraph.
    m_frameGraph->setCamera(m_camera);
    m_frameGraph->setBuffersToClear(Qt3DRender::QClearBuffers::AllBuffers);

    if(_is_qml)
    {
        m_inputSettings = new Qt3DInput::QInputSettings(this);
        this->addComponent(m_inputSettings);

        // Defines render settings allowing to define custom framegraphs.
        // Sets previously defined ForwardRenderer as default renderer.
        m_renderSettings = new Qt3DRender::QRenderSettings(this);
        m_renderSettings->setActiveFrameGraph(m_frameGraph);
        this->addComponent(m_renderSettings);
    }
}

//------------------------------------------------------------------------------

GenericScene::~GenericScene()
= default;

//------------------------------------------------------------------------------

Qt3DRender::QCamera* GenericScene::getCamera()
{
    return m_camera;
}

//------------------------------------------------------------------------------

Qt3DExtras::QAbstractCameraController* GenericScene::getCameraController()
{
    return m_cameraController;
}

//------------------------------------------------------------------------------

Qt3DInput::QInputSettings* GenericScene::getInputSettings()
{
    return m_inputSettings;
}

//------------------------------------------------------------------------------

Qt3DRender::QRenderSettings* GenericScene::getRenderSettings()
{
    return m_renderSettings;
}

//------------------------------------------------------------------------------

FrameGraph* GenericScene::getFrameGraph()
{
    return m_frameGraph;
}

//------------------------------------------------------------------------------

void GenericScene::setCamera(Qt3DRender::QCamera* _camera)
{
    SIGHT_ASSERT("Camera can't be set to null value.", _camera);
    m_camera = _camera;
    m_frameGraph->setCamera(_camera);
}

//------------------------------------------------------------------------------

void GenericScene::setCameraController(Qt3DExtras::QAbstractCameraController* _camera_controller)
{
    m_cameraController = _camera_controller;
}

//------------------------------------------------------------------------------

void GenericScene::SetInputSettings(Qt3DInput::QInputSettings* _input_settings)
{
    m_inputSettings = _input_settings;
}

//------------------------------------------------------------------------------

void GenericScene::setRenderSettings(Qt3DRender::QRenderSettings* _render_settings)
{
    m_renderSettings = _render_settings;
}

//------------------------------------------------------------------------------

void GenericScene::setFrameGraph(FrameGraph* _frame_graph)
{
    SIGHT_ASSERT("FrameGraph can't be set to null value.", _frame_graph);
    m_frameGraph = _frame_graph;
}

} // namespace sight::viz::qt3d::core
