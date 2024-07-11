/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

class frame_graph;

} // namespace core

/**
 * @brief This class creates a compute shader that changes a quad mesh to a triangle mesh.
 */
class SIGHT_VIZ_QT3D_CLASS_API compute_material : public Qt3DRender::QMaterial
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(Qt3DRender::QBuffer * buffer READ get_index_buffer WRITE set_index_buffer NOTIFY index_buffer_changed)

public:

    /// Creates the compute shader.
    SIGHT_VIZ_QT3D_API compute_material(Qt3DCore::QNode* _parent = nullptr);

    /// Destroy the material.
    SIGHT_VIZ_QT3D_API ~compute_material() override;

    /// Sets the index buffer of the mesh.
    SIGHT_VIZ_QT3D_API void set_index_buffer(Qt3DRender::QBuffer* _buffer);

    /// Gets the index buffer of the mesh.
    SIGHT_VIZ_QT3D_API Qt3DRender::QBuffer* get_index_buffer();

    /// Adds the material to the given frame graph and sets the work group of the compute shader.
    SIGHT_VIZ_QT3D_API void update_frame_graph(viz::qt3d::core::frame_graph* _frame_graph, int _number_of_cells);

Q_SIGNALS:

    /// Emitted when the index buffer is changed.
    void index_buffer_changed();

private:

    /// Contains the effect.
    Qt3DRender::QEffect* m_effect {nullptr};

    /// Contains the compute shader.
    Qt3DRender::QShaderProgram* m_compute_shader {nullptr};

    /// Contains the render pass.
    Qt3DRender::QRenderPass* m_compute_render_pass {nullptr};

    /// Contains the index buffer.
    Qt3DRender::QBuffer* m_index_buffer {nullptr};

    /// Contains the index buffer parameter.
    Qt3DRender::QParameter* m_index_buffer_parameter {nullptr};

    /// Contains the filter key.
    Qt3DRender::QFilterKey* m_compute_filter_key {nullptr};

    /// Contains the compute technique.
    Qt3DRender::QTechnique* m_compute_technique {nullptr};
};

//------------------------------------------------------------------------------

inline Qt3DRender::QBuffer* compute_material::get_index_buffer()
{
    return m_index_buffer;
}

} //namespace sight::viz::qt3d.
