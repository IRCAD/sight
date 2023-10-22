/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <QMaterial>

namespace Qt3DRender
{

class QBuffer;
class QEffect;
class QFilterKey;
class QParameter;
class QRenderPass;
class QShaderProgram;
class QTechnique;

} // namespace Qt3DRender

namespace sight::viz::qt3d
{

namespace core
{

class FrameGraph;

} // namespace core

/**
 * @brief This class creates a compute shader that changes a quad mesh to a triangle mesh.
 */
class VIZ_QT3D_CLASS_API ComputeMaterial : public Qt3DRender::QMaterial
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(Qt3DRender::QBuffer* buffer READ getIndexBuffer WRITE setIndexBuffer NOTIFY indexBufferChanged)

public:

    /// Creates the compute shader.
    VIZ_QT3D_API ComputeMaterial(Qt3DCore::QNode* _parent = nullptr);

    /// Destroy the material.
    VIZ_QT3D_API ~ComputeMaterial() override;

    /// Sets the index buffer of the mesh.
    VIZ_QT3D_API void setIndexBuffer(Qt3DRender::QBuffer* _buffer);

    /// Gets the index buffer of the mesh.
    VIZ_QT3D_API Qt3DRender::QBuffer* getIndexBuffer();

    /// Adds the material to the given frame graph and sets the work group of the compute shader.
    VIZ_QT3D_API void updateFrameGraph(viz::qt3d::core::FrameGraph* _frame_graph, int _number_of_cells);

Q_SIGNALS:

    /// Emitted when the index buffer is changed.
    void indexBufferChanged();

private:

    /// Contains the effect.
    Qt3DRender::QEffect* m_effect {nullptr};

    /// Contains the compute shader.
    Qt3DRender::QShaderProgram* m_computeShader {nullptr};

    /// Contains the render pass.
    Qt3DRender::QRenderPass* m_computeRenderPass {nullptr};

    /// Contains the index buffer.
    Qt3DRender::QBuffer* m_indexBuffer {nullptr};

    /// Contains the index buffer parameter.
    Qt3DRender::QParameter* m_indexBufferParameter {nullptr};

    /// Contains the filter key.
    Qt3DRender::QFilterKey* m_computeFilterKey {nullptr};

    /// Contains the compute technique.
    Qt3DRender::QTechnique* m_computeTechnique {nullptr};
};

//------------------------------------------------------------------------------

inline Qt3DRender::QBuffer* ComputeMaterial::getIndexBuffer()
{
    return m_indexBuffer;
}

} //namespace sight::viz::qt3d.
