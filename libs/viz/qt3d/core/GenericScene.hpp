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

#pragma once

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/core/FrameGraph.hpp"

#include <QPointer>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QAbstractCameraController>

#include <Qt3DInput/QInputSettings>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

namespace sight::viz::qt3d::core
{

/**
 * @brief Manages a generic scene.
 *
 * A generic scene is the root component of the scene hierarchy. Entities to be drawn must be declared as children of a
 * scene.
 * A scene contains a camera, a camera controller and a framegraph. In a QML application, GenericScene also defines a
 * QInputSettings object and a QRenderSettings object needed by QML.
 */
class VIZ_QT3D_CLASS_QT_API GenericScene : public Qt3DCore::QEntity
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    Qt3DRender::QCamera* camera
    READ getCamera WRITE setCamera NOTIFY cameraChanged
)
Q_PROPERTY(
    Qt3DExtras::QAbstractCameraController* cameraController
    READ getCameraController WRITE setCameraController NOTIFY cameraControllerChanged
)
Q_PROPERTY(
    Qt3DInput::QInputSettings* inputSettings
    READ getInputSettings WRITE setInputSettings NOTIFY inputSettingsChanged
)
Q_PROPERTY(
    Qt3DRender::QRenderSettings* renderSettings
    READ getRenderSettings WRITE setRenderSettings NOTIFY renderSettingsChanged
)

public:

    /// Constructs a scene.
    VIZ_QT3D_QT_API GenericScene(bool _isQml = true, Qt3DCore::QNode* _parent = nullptr);

    /// Destructs a scene.
    VIZ_QT3D_QT_API ~GenericScene() override;

    /// @returns the scene's camera.
    VIZ_QT3D_QT_API Qt3DRender::QCamera* getCamera();

    /// @returns the scene's camera controller.
    VIZ_QT3D_QT_API Qt3DExtras::QAbstractCameraController* getCameraController();

    /// @returns the scene's QInputSettings object (only used in QML applications).
    VIZ_QT3D_QT_API Qt3DInput::QInputSettings* getInputSettings();

    /// @returns the scene's render settings object (only used in QML applications).
    VIZ_QT3D_QT_API Qt3DRender::QRenderSettings* getRenderSettings();

    /// @returns the scene's framegraph.
    VIZ_QT3D_QT_API FrameGraph* getFrameGraph();

    /// Updates the scene's camera.
    VIZ_QT3D_QT_API void setCamera(Qt3DRender::QCamera* _camera);

    /// Updates the scene's camera controller.
    VIZ_QT3D_QT_API void setCameraController(Qt3DExtras::QAbstractCameraController* _cameraController);

    /// Updates the scene's QInputSettings object (only used in QML applications).
    VIZ_QT3D_QT_API void setInputSettings(Qt3DInput::QInputSettings* _inputSettings);

    /// Updates the scene's render settings object (only used in QML applications).
    VIZ_QT3D_QT_API void setRenderSettings(Qt3DRender::QRenderSettings* _renderSettings);

    /// Updates the scene's framegraph.
    VIZ_QT3D_QT_API void setFrameGraph(FrameGraph* _frameGraph);

Q_SIGNALS:

    /// Signal emitted when camera is modified.
    void cameraChanged();

    /// Signal emitted when camera controller is modified.
    void cameraControllerChanged();

    /// Signal emitted when QInputSettings object is modified.
    void inputSettingsChanged();

    /// Signal emitted when camera is modified.
    void renderSettingsChanged();

private:

    /// Contains the scene's camera.
    QPointer<Qt3DRender::QCamera> m_camera;

    /// Contains the scene's camera controller.
    QPointer<Qt3DExtras::QAbstractCameraController> m_cameraController;

    /// Contains the scene's QInputSettings object (only used in QML applications).
    QPointer<Qt3DInput::QInputSettings> m_inputSettings;

    /// Contains the scene's QRenderSettings object (only used in QML applications).
    QPointer<Qt3DRender::QRenderSettings> m_renderSettings;

    /// Contains the scene's framegraph.
    QPointer<FrameGraph> m_frameGraph;
};

} // namespace sight::viz::qt3d::core
