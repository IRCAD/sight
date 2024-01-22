/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/data/material.hpp"

#include <data/mesh.hpp>

#include <QPointer>

#include <Qt3DCore/QEntity>

#include <QVector3D>

namespace Qt3DRender
{

class QAttribute;
class QBuffer;
class QGeometryRenderer;
class QGeometry;

} // namespace Qt3DRender

namespace sight::viz::qt3d
{

namespace core
{

class generic_scene;

} // namespace core

namespace data
{

/**
 * @brief Manages a generic mesh.
 */
class VIZ_QT3D_CLASS_API_QT mesh : public Qt3DCore::QEntity
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(material * material READ get_material WRITE set_material NOTIFY material_changed)

public:

    /// Constructs an empty mesh.
    VIZ_QT3D_API_QT mesh(Qt3DCore::QNode* _parent = nullptr);

    /// Destroys the mesh.
    VIZ_QT3D_API_QT ~mesh() override;

    /// @returns mesh material.
    [[nodiscard]] VIZ_QT3D_API_QT viz::qt3d::data::material* get_material() const;

    /// @returns the scene associated with the mesh.
    [[nodiscard]] VIZ_QT3D_API_QT sight::viz::qt3d::core::generic_scene* get_scene() const;

    /// Updates mesh material.
    VIZ_QT3D_API_QT void set_material(viz::qt3d::data::material* _material);

    /// Updates the scene associated with the mesh.
    VIZ_QT3D_API_QT void set_scene(sight::viz::qt3d::core::generic_scene* _scene);

    /// Constructs position and normal buffers according to _mesh.
    VIZ_QT3D_API_QT void build_buffers(sight::data::mesh::sptr _mesh);

    /// Updates the mesh according to _mesh.
    VIZ_QT3D_API_QT Q_INVOKABLE void set_mesh(sight::data::mesh::sptr _mesh);

    /// Centers camera on mesh.
    VIZ_QT3D_API_QT Q_INVOKABLE void center_camera_on_mesh();

Q_SIGNALS:

    /// Signal emitted when mesh material is modified.
    void material_changed();

private:

    /**
     * @brief Adds the compute shader that change a quad mesh to a triangle mesh.
     * @param _number_of_cells the cells number of the mesh.
     */
    void add_compute_entity_to_scene(int _number_of_cells);

    /// Contains the scene rendering this mesh.
    QPointer<sight::viz::qt3d::core::generic_scene> m_scene;

    /// Contains QGeometryRenderer object holding informations needed to render a QGeometry object.
    QPointer<Qt3DRender::QGeometryRenderer> m_geom_renderer;

    /// Contains mesh geometry.
    QPointer<Qt3DRender::QGeometry> m_geometry;

    /// Contains mesh material.
    QPointer<viz::qt3d::data::material> m_material;

    /// Contains mesh position attribute.
    QPointer<Qt3DRender::QAttribute> m_pos_attrib;

    /// Contains mesh normal attribute.
    QPointer<Qt3DRender::QAttribute> m_normal_attrib;

    /// Contains mesh index attribute.
    QPointer<Qt3DRender::QAttribute> m_index_attrib;

    /// Contains mesh position buffer.
    QPointer<Qt3DRender::QBuffer> m_pos_buffer;

    /// Contains mesh normal buffer.
    QPointer<Qt3DRender::QBuffer> m_normal_buffer;

    /// Contains mesh index buffer.
    QPointer<Qt3DRender::QBuffer> m_index_buffer;

    /// Specifies number of points in the mesh.
    unsigned int m_number_of_points {0};

    /// Specifies the number of components of a vertex (3 for a 3D vertex).
    unsigned int m_vertex_size {3};

    /// Specifies the mesh center. Needed to center camera.
    QVector3D m_mesh_center;

    /// Specifies minimum extents of the mesh. Needed to center camera.
    QVector3D m_min_extent;

    /// Specifies maximum extents of the mesh. Needed to center camera.
    QVector3D m_max_extent;
};

} // namespace data.

} // namespace sight::viz::qt3d.
