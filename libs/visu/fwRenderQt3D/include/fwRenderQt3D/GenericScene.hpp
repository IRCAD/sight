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

#pragma once

#include "fwRenderQt3D/config.hpp"

#include <Qt3DCore/QEntity>

// Forward declarations
namespace Qt3DRender
{
class QCamera;
class QRenderSettings;
}

namespace Qt3DExtras
{
class QAbstractCameraController;
class QForwardRenderer;
}

namespace Qt3DInput
{
class QInputSettings;
}

namespace fwRenderQt3D
{

// This class contains the Qt3D scene.
// GenericScene object represents scene's root, objects can be added to the scene using addComponent() method inherithed
// from Qt3DCore::QEntity
// The scene contains some default component, see appropriate documentation for more infos:
//      - QCamera object implementing the scene's camera.
//      - QAbstractCameraController object implementing camera's controller. By default it is a QOrbitCameraController.
//      - QInputSettings object implementing mouse events, thus allowing to effectivly control the camera
//      - QRenderSettings object, implementing rendering options, such as the framegraph used to render the scene.
// These objects can be accessed through QML or C++ using appropriate properties/methods.
class FWRENDERQT3D_CLASS_API GenericScene : public Qt3DCore::QEntity
{
// Q_PROPERTY macros associate scene objects with QML properties
Q_OBJECT
Q_PROPERTY(Qt3DRender::QCamera* camera READ getCamera WRITE setCamera NOTIFY cameraChanged)
Q_PROPERTY(
    Qt3DExtras::QAbstractCameraController* cameraController READ getCameraController WRITE setCameraController NOTIFY cameraControllerChanged)
Q_PROPERTY(
    Qt3DInput::QInputSettings* inputSettings READ getInputSettings WRITE setInputSettings NOTIFY inputSettingsChanged)
Q_PROPERTY(
    Qt3DRender::QRenderSettings* renderSettings READ getRenderSettings WRITE setRenderSettings NOTIFY renderSettingsChanged)
Q_PROPERTY(Qt3DExtras::QForwardRenderer* defaultFrameGraph READ getDefaultFrameGraph)

public:
    // Constructor / Destructor
    FWRENDERQT3D_API GenericScene(bool _isQml = true, Qt3DCore::QNode* parent = 0);
    FWRENDERQT3D_API virtual ~GenericScene();

    // Getters
    FWRENDERQT3D_API Qt3DRender::QCamera* getCamera();
    FWRENDERQT3D_API Qt3DExtras::QAbstractCameraController* getCameraController();
    FWRENDERQT3D_API Qt3DInput::QInputSettings* getInputSettings();
    FWRENDERQT3D_API Qt3DRender::QRenderSettings* getRenderSettings();

    // Q_INVOKABLE macro allows to access this method in QML files using JavaScript
    FWRENDERQT3D_API Q_INVOKABLE Qt3DExtras::QForwardRenderer* getDefaultFrameGraph();

    // Setters
    FWRENDERQT3D_API void setCamera(Qt3DRender::QCamera* _camera);
    FWRENDERQT3D_API void setCameraController(Qt3DExtras::QAbstractCameraController* _cameraController);
    FWRENDERQT3D_API void setInputSettings(Qt3DInput::QInputSettings* _inputSettings);
    FWRENDERQT3D_API void setRenderSettings(Qt3DRender::QRenderSettings* _renderSettings);

Q_SIGNALS:
    // Signals emitted when respective objects are modified
    void cameraChanged();
    void cameraControllerChanged();
    void inputSettingsChanged();
    void renderSettingsChanged();

private:
    Qt3DRender::QCamera* m_camera;
    Qt3DExtras::QAbstractCameraController* m_cameraController;
    Qt3DInput::QInputSettings* m_inputSettings;
    Qt3DRender::QRenderSettings* m_renderSettings;

    Qt3DExtras::QForwardRenderer* m_defaultFrameGraph;
};

}