/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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
#include "viz/qt3d/data/Material.hpp"

#include <data/Mesh.hpp>

#include <QPointer>

#include <Qt3DCore/QEntity>

#include <QVector3D>

namespace Qt3DRender
{
class QAttribute;
class QBuffer;
class QGeometryRenderer;
class QGeometry;
}

namespace sight::viz::qt3d
{

namespace core
{
class GenericScene;
}

namespace data
{

/**
 * @brief Manages a generic mesh.
 */
class VIZ_QT3D_CLASS_QT_API Mesh : public Qt3DCore::QEntity
{

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    Material* material
    READ getMaterial WRITE setMaterial NOTIFY materialChanged)

public:

    /// Constructs an empty mesh.
    VIZ_QT3D_QT_API Mesh(Qt3DCore::QNode* _parent = nullptr);

    /// Destroys the mesh.
    VIZ_QT3D_QT_API ~Mesh() override;

    /// @returns mesh material.
    VIZ_QT3D_QT_API viz::qt3d::data::Material* const getMaterial() const;

    /// @returns the scene associated with the mesh.
    VIZ_QT3D_QT_API sight::viz::qt3d::core::GenericScene* const getScene() const;

    /// Updates mesh material.
    VIZ_QT3D_QT_API void setMaterial(viz::qt3d::data::Material* _material);

    /// Updates the scene associated with the mesh.
    VIZ_QT3D_QT_API void setScene(sight::viz::qt3d::core::GenericScene* _scene);

    /// Constructs postion and normal buffers according to _mesh.
    VIZ_QT3D_QT_API void buildBuffers(sight::data::Mesh::sptr _mesh);

    /// Updates the mesh according to _mesh.
    VIZ_QT3D_QT_API Q_INVOKABLE void setMesh(sight::data::Mesh::sptr _mesh);

    /// Centers camera on mesh.
    VIZ_QT3D_QT_API Q_INVOKABLE void centerCameraOnMesh();

Q_SIGNALS:

    /// Signal emitted when mesh material is modified.
    void materialChanged();

private:

    /// Contains the scene rendering this mesh.
    QPointer< sight::viz::qt3d::core::GenericScene > m_scene;

    /// Contains QGeometryRenderer object holding informations needed to render a QGeometry object.
    QPointer< Qt3DRender::QGeometryRenderer > m_geomRenderer;

    /// Contains mesh geometry.
    QPointer< Qt3DRender::QGeometry > m_geometry;

    /// Contains mesh material.
    QPointer< viz::qt3d::data::Material > m_material;

    /// Contains mesh position attribute.
    QPointer< Qt3DRender::QAttribute > m_posAttrib;

    /// Contains mesh normal attribute.
    QPointer< Qt3DRender::QAttribute > m_normalAttrib;

    /// Contains mesh index attribute.
    QPointer< Qt3DRender::QAttribute > m_indexAttrib;

    /// Contains mesh position buffer.
    QPointer< Qt3DRender::QBuffer > m_posBuffer;

    /// Contains mesh normal buffer.
    QPointer< Qt3DRender::QBuffer > m_normalBuffer;

    /// Contains mesh index buffer.
    QPointer< Qt3DRender::QBuffer > m_indexBuffer;

    /// Specifies number of points in the mesh.
    unsigned int m_numberOfPoints { 0 };

    /// Specifies the number of components of a vertex (3 for a 3D vertex).
    unsigned int m_vertexSize { 3 };

    /// Specifies the mesh center. Needed to center camera.
    QVector3D m_meshCenter;

    /// Specifies minimum extents of the mesh. Needed to center camera.
    QVector3D m_minExtent;

    /// Specifies maximum extents of the mesh. Needed to center camera.
    QVector3D m_maxExtent;

};

} // namespace data.

} // namespace sight::viz::qt3d.
