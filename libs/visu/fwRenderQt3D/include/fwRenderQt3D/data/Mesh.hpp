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

#include <fwData/Mesh.hpp>

#include <QPointer>

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QMaterial>

#include <QVector3D>

namespace Qt3DRender
{
class QAttribute;
class QBuffer;
class QGeometryRenderer;
class QGeometry;
}

namespace fwRenderQt3D
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
class FWRENDERQT3D_CLASS_QT_API Mesh : public Qt3DCore::QEntity
{

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(
    Qt3DRender::QMaterial* material
    READ getMaterial WRITE setMaterial NOTIFY materialChanged)

public:

    /// Constructs an empty mesh.
    FWRENDERQT3D_QT_API Mesh(Qt3DCore::QNode* _parent = nullptr);

    /// Constructs a mesh from _mesh.
    FWRENDERQT3D_QT_API Mesh(::fwData::Mesh::sptr _mesh, Qt3DCore::QNode* _parent = nullptr);

    /// Destroys the mesh.
    FWRENDERQT3D_QT_API ~Mesh() override;

    /// @returns mesh material.
    FWRENDERQT3D_QT_API Qt3DRender::QMaterial* const getMaterial() const;

    /// @returns the scene associated with the mesh.
    FWRENDERQT3D_QT_API ::fwRenderQt3D::core::GenericScene* const getScene() const;

    /// Updates mesh material.
    FWRENDERQT3D_QT_API void setMaterial(Qt3DRender::QMaterial* _material);

    /// Updates the scene associated with the mesh.
    FWRENDERQT3D_QT_API void setScene(::fwRenderQt3D::core::GenericScene* _scene);

    /// Updates the mesh according to _mesh.
    FWRENDERQT3D_QT_API Q_INVOKABLE void setMesh(::fwData::Mesh::sptr _mesh);

    /// Centers camera on mesh.
    FWRENDERQT3D_QT_API Q_INVOKABLE void centerCameraOnMesh();

    /// Constructs postion and normal buffers according to _mesh.
    FWRENDERQT3D_QT_API Q_INVOKABLE void buildBuffers(::fwData::Mesh::sptr _mesh);

Q_SIGNALS:

    /// Signal emitted when mesh material is modified.
    void materialChanged();

private:

    /// Contains the scene rendering this mesh.
    QPointer< ::fwRenderQt3D::core::GenericScene > m_scene;

    /// Contains QGeometryRenderer object holding informations needed to render a QGeometry object.
    QPointer< Qt3DRender::QGeometryRenderer > m_geomRenderer;

    /// Contains mesh geometry.
    QPointer< Qt3DRender::QGeometry > m_geometry;

    /// Contains mesh material.
    QPointer< Qt3DRender::QMaterial > m_material;

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

} // namespace fwRenderQt3D.
