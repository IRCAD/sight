/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/viz/qt3d/config.hpp>

#include <QClearBuffers>
#include <QFrameGraphNode>
#include <QPointer>

namespace Qt3DRender
{

class QCameraSelector;
class QRenderStateSet;
class QRenderSurfaceSelector;
class QViewport;

} // namespace Qt3DRender

namespace sight::viz::qt3d::core
{

/**
 * @brief Manages a qt3d framegraph.
 *
 * The framegraph specifies how the scene will be rendered.
 * This class contains a basic framegraph performing a forward rendering after clearing buffers if specified.
 */
class SIGHT_VIZ_QT3D_CLASS_API_QT frame_graph : public Qt3DRender::QFrameGraphNode
{
/// These typedefs are used for Q_PROPERTY, the macro doesn't work with namespaces.
using q_entity        = Qt3DCore::QEntity;
using q_clear_buffers = Qt3DRender::QClearBuffers;

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    q_entity * camera
    READ get_camera WRITE set_camera NOTIFY camera_changed
)
Q_PROPERTY(
    QColor clearColor
    READ get_clear_color WRITE set_clear_color NOTIFY clear_color_changed
)
Q_PROPERTY(
    q_clear_buffers::BufferType buffersToClear
    READ get_buffers_to_clear WRITE set_buffers_to_clear NOTIFY buffers_to_clear_changed
)

public:

    /// Constructs the framegraph.
    SIGHT_VIZ_QT3D_API_QT frame_graph(Qt3DCore::QNode* _parent = nullptr);

    /// Destructs the framegraph.
    SIGHT_VIZ_QT3D_API_QT ~frame_graph() override;

    /// @returns the camera this framegraph is associated to.
    [[nodiscard]] SIGHT_VIZ_QT3D_API_QT Qt3DCore::QEntity* get_camera() const;

    /// @returns the framegraph's clear color.
    [[nodiscard]] SIGHT_VIZ_QT3D_API_QT QColor get_clear_color() const;

    /// @returns the buffers to clear before rendering.
    [[nodiscard]] SIGHT_VIZ_QT3D_API_QT Qt3DRender::QClearBuffers::BufferType get_buffers_to_clear() const;

    /// Gets the camera selector.
    SIGHT_VIZ_QT3D_API_QT Qt3DRender::QCameraSelector* get_camera_selector();

    /// Updates the framegraph's camera.
    SIGHT_VIZ_QT3D_API_QT void set_camera(Qt3DCore::QEntity* _camera);

    /// Sets color used to clear the scene before rendering.
    SIGHT_VIZ_QT3D_API_QT void set_clear_color(const QColor& _color);

    /// Sets the buffers to clear before rendering.
    SIGHT_VIZ_QT3D_API_QT void set_buffers_to_clear(Qt3DRender::QClearBuffers::BufferType _buffers);

    /// Gets all nodes attached to the frame graph.
    SIGHT_VIZ_QT3D_API_QT QVector<Qt3DRender::QFrameGraphNode*> get_all_nodes();

    /// Adds a node to the frame graph.
    SIGHT_VIZ_QT3D_API_QT void add_node(Qt3DRender::QFrameGraphNode* _node, Qt3DRender::QFrameGraphNode* _parent);

Q_SIGNALS:

    /// Signal emitted when camera is modified.
    void camera_changed();

    /// Signal emitted when clear color is modified.
    void clear_color_changed();

    /// Signal emitted when the buffers to clear are changed.
    void buffers_to_clear_changed();

private:

    /// Gets recursively all nodes from the frame graph.
    void get_all_nodes_rec(QVector<Qt3DRender::QFrameGraphNode*>& _nodes, Qt3DRender::QFrameGraphNode* _current_node);

    ///Contains a viewport allowing to choose which portion of the screen is rendered. Default is the whole screen.
    QPointer<Qt3DRender::QViewport> m_viewport;

    /// Contains a camera selector to choose camera from which the scene will be rendered.
    QPointer<Qt3DRender::QCameraSelector> m_camera_selector;

    /// Contains a clear buffer object allowing to control clear color and which buffers are cleared before rendering.
    QPointer<Qt3DRender::QClearBuffers> m_clear_buffers;

    /// Contains a render surface selector. Needed to set a proper rendering context.
    QPointer<Qt3DRender::QRenderSurfaceSelector> m_render_surface_selector;

    /// Contains a render state set which can be filled with render states allowing to control options such as culling
    /// or depth test.
    QPointer<Qt3DRender::QRenderStateSet> m_render_state_set;
};

} // namespace sight::viz::qt3d::core
