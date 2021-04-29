/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <QClearBuffers>
#include <QFrameGraphNode>
#include <QPointer>

namespace Qt3DRender
{
class QCameraSelector;
class QRenderStateSet;
class QRenderSurfaceSelector;
class QViewport;
}

namespace sight::viz::qt3d
{

namespace core
{

/**
 * @brief Manages a qt3d framegraph.
 *
 * The framegraph specifies how the scene will be rendered.
 * This class contains a basic framegraph performing a forward rendering after clearing buffers if specified.
 */
class VIZ_QT3D_CLASS_QT_API FrameGraph : public Qt3DRender::QFrameGraphNode
{

/// These typedefs are used for Q_PROPERTY, the macro doesn't work with namespaces.
typedef Qt3DCore::QEntity QEntity;
typedef Qt3DRender::QClearBuffers QClearBuffers;

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    QEntity* camera
    READ getCamera WRITE setCamera NOTIFY cameraChanged)
Q_PROPERTY(
    QColor clearColor
    READ getClearColor WRITE setClearColor NOTIFY clearColorChanged)
Q_PROPERTY(
    QClearBuffers::BufferType buffersToClear
    READ getBuffersToClear WRITE setBuffersToClear NOTIFY buffersToClearChanged)

public:

    /// Constructs the framegraph.
    VIZ_QT3D_QT_API FrameGraph(Qt3DCore::QNode* _parent = nullptr);

    /// Destructs the framegraph.
    VIZ_QT3D_QT_API ~FrameGraph() override;

    /// @returns the camera this framegraph is associated to.
    VIZ_QT3D_QT_API Qt3DCore::QEntity* getCamera() const;

    /// @returns the framegraph's clear color.
    VIZ_QT3D_QT_API QColor getClearColor() const;

    /// @returns the buffers to clear before rendering.
    VIZ_QT3D_QT_API Qt3DRender::QClearBuffers::BufferType getBuffersToClear() const;

    /// Gets the camera selector.
    VIZ_QT3D_QT_API Qt3DRender::QCameraSelector* getCameraSelector();

    /// Updates the framegraph's camera.
    VIZ_QT3D_QT_API void setCamera(Qt3DCore::QEntity* _camera);

    /// Sets color used to clear the scene before rendering.
    VIZ_QT3D_QT_API void setClearColor(const QColor& _color);

    /// Sets the buffers to clear before rendering.
    VIZ_QT3D_QT_API void setBuffersToClear(Qt3DRender::QClearBuffers::BufferType _buffers);

    /// Gets all nodes attached to the frame graph.
    VIZ_QT3D_QT_API QVector<Qt3DRender::QFrameGraphNode*> getAllNodes();

    /// Adds a node to the frame graph.
    VIZ_QT3D_QT_API void addNode(Qt3DRender::QFrameGraphNode* _node, Qt3DRender::QFrameGraphNode* _parent);

Q_SIGNALS:

    /// Signal emitted when camera is modified.
    void cameraChanged();

    /// Signal emitted when clear color is modified.
    void clearColorChanged();

    /// Signal emitted when the buffers to clear are changed.
    void buffersToClearChanged();

private:

    /// Gets recursivly alls node from the frame graph.
    void getAllNodesRec(QVector<Qt3DRender::QFrameGraphNode*>& _nodes, Qt3DRender::QFrameGraphNode* _currentNode);

    ///Contains a viewport allowing to choose which portion of the screen is rendered. Default is the whole screen.
    QPointer< Qt3DRender::QViewport > m_viewport;

    /// Contains a camera selector to choose camera from which the scene will be rendered.
    QPointer< Qt3DRender::QCameraSelector > m_cameraSelector;

    /// Contains a clear buffer object allowing to control clear color and which buffers are cleared before rendering.
    QPointer< Qt3DRender::QClearBuffers > m_clearBuffers;

    /// Contains a render surface selector. Needed to set a proper rendering context.
    QPointer< Qt3DRender::QRenderSurfaceSelector > m_renderSurfaceSelector;

    /// Contains a render state set which can be filled with render states allowing to control options such as culling
    /// or depth test.
    QPointer< Qt3DRender::QRenderStateSet > m_renderStateSet;

};

} // namespace core.

} // namespace sight::viz::qt3d.
