/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "viz/qt3d/data/Mesh.hpp"

#include <QClearBuffers>
#include <QPhongMaterial>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QOrbitCameraController>

namespace sight::viz::qt3d::core
{

GenericScene::GenericScene(bool _isQml, Qt3DCore::QNode* _parent) :
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

    if(_isQml)
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

void GenericScene::setCameraController(Qt3DExtras::QAbstractCameraController* _cameraController)
{
    m_cameraController = _cameraController;
}

//------------------------------------------------------------------------------

void GenericScene::setInputSettings(Qt3DInput::QInputSettings* _inputSettings)
{
    m_inputSettings = _inputSettings;
}

//------------------------------------------------------------------------------

void GenericScene::setRenderSettings(Qt3DRender::QRenderSettings* _renderSettings)
{
    m_renderSettings = _renderSettings;
}

//------------------------------------------------------------------------------

void GenericScene::setFrameGraph(FrameGraph* _frameGraph)
{
    SIGHT_ASSERT("FrameGraph can't be set to null value.", _frameGraph);
    m_frameGraph = _frameGraph;
}

} // namespace sight::viz::qt3d::core
