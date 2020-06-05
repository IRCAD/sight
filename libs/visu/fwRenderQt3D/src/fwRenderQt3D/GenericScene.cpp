/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwRenderQt3D/GenericScene.hpp"

#include <QPhongMaterial>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DInput/QInputSettings>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

namespace fwRenderQt3D
{

GenericScene::GenericScene(bool _isQml, Qt3DCore::QNode* parent) :
    QEntity(parent)
{
    // Defines the camera that will be used to render te scene
    m_camera = new Qt3DRender::QCamera(this);
    m_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_camera->setPosition(QVector3D(0, 10.0f, 40.0f));
    m_camera->setViewCenter(QVector3D(0, 0, 0));

    // Defines a camera controller allowing user to control camera with the mouse
    // and sets it as a child of the scene entity
    m_cameraController = new Qt3DExtras::QOrbitCameraController(this);
    m_cameraController->setCamera(m_camera);
    m_cameraController->setLinearSpeed(50.0f);
    m_cameraController->setLookSpeed(180.0f);

    Qt3DCore::QEntity* const cylinderEntity = new Qt3DCore::QEntity(this);

    Qt3DExtras::QCylinderMesh* const cylinderMesh = new Qt3DExtras::QCylinderMesh(this);
    cylinderMesh->setLength(10);
    cylinderMesh->setRadius(2);
    cylinderMesh->setRings(100);
    cylinderMesh->setSlices(20);

    Qt3DExtras::QPhongMaterial* const cylinderMaterial = new Qt3DExtras::QPhongMaterial(this);
    cylinderMaterial->setAmbient(QColor(150, 0, 100));

    cylinderEntity->addComponent(cylinderMesh);
    cylinderEntity->addComponent(cylinderMaterial);

    if(_isQml)
    {
        m_inputSettings = new Qt3DInput::QInputSettings(this);
        this->addComponent(m_inputSettings);

        // Define default renderer
        m_defaultFrameGraph = new Qt3DExtras::QForwardRenderer();
        m_defaultFrameGraph->setClearColor("#2d2d2d");
        m_defaultFrameGraph->setCamera(m_camera);

        // Defines render settings allowing to define custom framegraphs
        // Sets previously defined ForwardRenderer as default renderer
        m_renderSettings = new Qt3DRender::QRenderSettings(this);
        m_renderSettings->setActiveFrameGraph(m_defaultFrameGraph);
        this->addComponent(m_renderSettings);
    }
}

GenericScene::~GenericScene()
{
}

// --- GETTERS ---

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

// --- SETTERS ---

void GenericScene::setCamera(Qt3DRender::QCamera* _camera)
{
    m_camera = _camera;
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

// ---------------------------

Qt3DExtras::QForwardRenderer* GenericScene::getDefaultFrameGraph()
{
    return m_defaultFrameGraph;
}

}