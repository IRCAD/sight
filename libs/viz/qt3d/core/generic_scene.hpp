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

#pragma once

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/core/frame_graph.hpp"

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
 * A scene contains a camera, a camera controller and a framegraph. In a QML application, generic_scene also defines a
 * QInputSettings object and a QRenderSettings object needed by QML.
 */
class VIZ_QT3D_CLASS_API_QT generic_scene : public Qt3DCore::QEntity
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    Qt3DRender::QCamera* camera
    READ get_camera WRITE set_camera NOTIFY camera_changed
)
Q_PROPERTY(
    Qt3DExtras::QAbstractCameraController* camera_controller
    READ get_camera_controller WRITE set_camera_controller NOTIFY camera_controller_changed
)
Q_PROPERTY(
    Qt3DInput::QInputSettings* input_settings
    READ get_input_settings WRITE set_input_settings NOTIFY input_settings_changed
)
Q_PROPERTY(
    Qt3DRender::QRenderSettings* render_settings
    READ get_render_settings WRITE set_render_settings NOTIFY render_settings_changed
)

public:

    /// Constructs a scene.
    VIZ_QT3D_API_QT generic_scene(bool _is_qml = true, Qt3DCore::QNode* _parent = nullptr);

    /// Destructs a scene.
    VIZ_QT3D_API_QT ~generic_scene() override;

    /// @returns the scene's camera.
    VIZ_QT3D_API_QT Qt3DRender::QCamera* get_camera();

    /// @returns the scene's camera controller.
    VIZ_QT3D_API_QT Qt3DExtras::QAbstractCameraController* get_camera_controller();

    /// @returns the scene's QInputSettings object (only used in QML applications).
    VIZ_QT3D_API_QT Qt3DInput::QInputSettings* get_input_settings();

    /// @returns the scene's render settings object (only used in QML applications).
    VIZ_QT3D_API_QT Qt3DRender::QRenderSettings* get_render_settings();

    /// @returns the scene's framegraph.
    VIZ_QT3D_API_QT frame_graph* get_frame_graph();

    /// Updates the scene's camera.
    VIZ_QT3D_API_QT void set_camera(Qt3DRender::QCamera* _camera);

    /// Updates the scene's camera controller.
    VIZ_QT3D_API_QT void set_camera_controller(Qt3DExtras::QAbstractCameraController* _camera_controller);

    /// Updates the scene's QInputSettings object (only used in QML applications).
    VIZ_QT3D_API_QT void set_input_settings(Qt3DInput::QInputSettings* _input_settings);

    /// Updates the scene's render settings object (only used in QML applications).
    VIZ_QT3D_API_QT void set_render_settings(Qt3DRender::QRenderSettings* _render_settings);

    /// Updates the scene's framegraph.
    VIZ_QT3D_API_QT void set_frame_graph(frame_graph* _frame_graph);

Q_SIGNALS:

    /// Signal emitted when camera is modified.
    void camera_changed();

    /// Signal emitted when camera controller is modified.
    void camera_controller_changed();

    /// Signal emitted when QInputSettings object is modified.
    void input_settings_changed();

    /// Signal emitted when camera is modified.
    void render_settings_changed();

private:

    /// Contains the scene's camera.
    QPointer<Qt3DRender::QCamera> m_camera;

    /// Contains the scene's camera controller.
    QPointer<Qt3DExtras::QAbstractCameraController> m_camera_controller;

    /// Contains the scene's QInputSettings object (only used in QML applications).
    QPointer<Qt3DInput::QInputSettings> m_input_settings;

    /// Contains the scene's QRenderSettings object (only used in QML applications).
    QPointer<Qt3DRender::QRenderSettings> m_render_settings;

    /// Contains the scene's framegraph.
    QPointer<frame_graph> m_frame_graph;
};

} // namespace sight::viz::qt3d::core
