/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "viz/qt3d/data/mesh.hpp"

#include "viz/qt3d/ComputeMaterial.hpp"
#include "viz/qt3d/core/GenericScene.hpp"

#include <geometry/data/mesh.hpp>

#include <QComputeCommand>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

namespace sight::viz::qt3d::data
{

mesh::mesh(Qt3DCore::QNode* _parent) :
    QEntity(_parent),
    m_scene(qobject_cast<sight::viz::qt3d::core::GenericScene*>(_parent)),
    m_geomRenderer(new Qt3DRender::QGeometryRenderer(m_scene)),
    m_geometry(new Qt3DRender::QGeometry(m_geomRenderer)),
    m_material(new viz::qt3d::data::material()),
    m_posAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_normalAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_indexAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_posBuffer(new Qt3DRender::QBuffer(m_geometry)),
    m_normalBuffer(new Qt3DRender::QBuffer(m_geometry)),
    m_indexBuffer(new Qt3DRender::QBuffer(m_geometry))
{
    this->addComponent(m_geomRenderer);
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

mesh::~mesh()
= default;

//------------------------------------------------------------------------------

viz::qt3d::data::material* mesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::GenericScene* mesh::getScene() const
{
    return m_scene;
}

//------------------------------------------------------------------------------

void mesh::setMaterial(viz::qt3d::data::material* _material)
{
    SIGHT_ASSERT("Material can't be set to null value.", _material);
    this->removeComponent(m_material);
    m_material = _material;
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

void mesh::setScene(sight::viz::qt3d::core::GenericScene* _scene)
{
    SIGHT_ASSERT("scene can't be set to null value.", _scene);
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void mesh::setMesh(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Sets the number of points (for a mesh of triangles).
    m_numberOfPoints = static_cast<unsigned int>(_mesh->numPoints());

    if(!m_geometry->attributes().empty())
    {
        m_geometry->removeAttribute(m_posAttrib);
        m_geometry->removeAttribute(m_normalAttrib);
        m_geometry->removeAttribute(m_indexAttrib);
    }

    // Configures rendering attributes.
    m_posAttrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_posAttrib->setBuffer(m_posBuffer);
    m_posAttrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_posAttrib->setVertexSize(m_vertexSize);
    m_posAttrib->setByteOffset(0);
    m_posAttrib->setByteStride(3 * sizeof(float));
    m_posAttrib->setCount(m_numberOfPoints);
    m_posAttrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    m_geometry->addAttribute(m_posAttrib);

    m_normalAttrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normalAttrib->setBuffer(m_normalBuffer);
    m_normalAttrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_normalAttrib->setVertexSize(m_vertexSize);
    m_normalAttrib->setByteOffset(0);
    m_normalAttrib->setByteStride(3 * sizeof(float));
    m_normalAttrib->setCount(m_numberOfPoints);
    m_normalAttrib->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    m_geometry->addAttribute(m_normalAttrib);

    m_indexAttrib->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    m_indexAttrib->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    m_indexAttrib->setBuffer(m_indexBuffer);
    m_indexAttrib->setCount(3 * static_cast<unsigned int>(_mesh->numCells()));
    m_indexAttrib->setName("indexAttribute");

    m_geometry->addAttribute(m_indexAttrib);

    // Fills buffers with data extracted from _mesh.
    this->buildBuffers(_mesh);

    m_geometry->setBoundingVolumePositionAttribute(m_posAttrib);

    m_geomRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_geomRenderer->setGeometry(m_geometry);
}

//------------------------------------------------------------------------------

void mesh::centerCameraOnMesh()
{
    auto* camera = m_scene->getCamera();
    camera->viewSphere(
        m_meshCenter,
        std::max(
            std::max(m_maxExtent.x() - m_minExtent.x(), m_maxExtent.y() - m_minExtent.y()),
            m_maxExtent.z() - m_minExtent.z()
        )
    );
}

//------------------------------------------------------------------------------

void mesh::buildBuffers(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Returns if _mesh is empty.
    if(static_cast<int>(_mesh->numCells()) == 0 || static_cast<int>(_mesh->numPoints()) == 0)
    {
        return;
    }

    // Declares data arrays which are associated with QBuffers.
    QByteArray pos_buffer_data;
    pos_buffer_data.resize(static_cast<int>(static_cast<std::size_t>(m_numberOfPoints) * m_vertexSize * sizeof(float)));
    auto* raw_pos_buffer_data = reinterpret_cast<float*>(pos_buffer_data.data());

    QByteArray normal_buffer_data;
    normal_buffer_data.resize(static_cast<int>(std::size_t(m_numberOfPoints) * m_vertexSize * sizeof(float)));
    auto* raw_normal_buffer_data = reinterpret_cast<float*>(normal_buffer_data.data());

    QByteArray index_buffer_data;
    index_buffer_data.resize(3 * static_cast<int>(_mesh->numCells()) * static_cast<int>(sizeof(unsigned int)));
    auto* raw_index_buffer_data = reinterpret_cast<unsigned int*>(index_buffer_data.data());

    // Checks if the mesh has normals. If not, generates them.
    if(!_mesh->has<sight::data::mesh::Attributes::POINT_NORMALS>())
    {
        geometry::data::mesh::generatePointNormals(_mesh);
    }

    float min_x      = 0;
    float max_x      = 0;
    float min_y      = 0;
    float max_y      = 0;
    float min_z      = 0;
    float max_z      = 0;
    bool min_max_set = false;

    // Iterates over points and registers each point in position and normal buffers.
    namespace point = sight::data::iterator::point;
    namespace cell  = sight::data::iterator::cell;

    unsigned int count_pts = 0;
    for(const auto& [p, n] : _mesh->czip_range<point::xyz, point::nxyz>())
    {
        raw_pos_buffer_data[count_pts]     = p.x;
        raw_pos_buffer_data[count_pts + 1] = p.y;
        raw_pos_buffer_data[count_pts + 2] = p.z;

        raw_normal_buffer_data[count_pts]     = n.nx;
        raw_normal_buffer_data[count_pts + 1] = n.ny;
        raw_normal_buffer_data[count_pts + 2] = n.nz;

        count_pts += 3;

        // Computes mesh extents and center. Needed to center camera.
        if(!min_max_set)
        {
            min_x       = p.x;
            max_x       = p.x;
            min_y       = p.y;
            max_y       = p.y;
            min_z       = p.z;
            max_z       = p.z;
            min_max_set = true;
        }
        else
        {
            if(p.x < min_x)
            {
                min_x = p.x;
            }

            if(p.x > max_x)
            {
                max_x = p.x;
            }

            if(p.y < min_y)
            {
                min_y = p.y;
            }

            if(p.y > max_y)
            {
                max_y = p.y;
            }

            if(p.z < min_z)
            {
                max_z = p.z;
            }

            if(p.z > max_z)
            {
                max_z = p.z;
            }
        }
    }

    m_minExtent = QVector3D(min_x, min_y, min_z);
    m_maxExtent = QVector3D(max_x, max_y, max_z);

    m_meshCenter =
        QVector3D(
            (m_minExtent.x() + m_maxExtent.x()) / 2,
            (m_minExtent.y() + m_maxExtent.y()) / 2,
            (m_minExtent.z() + m_maxExtent.z()) / 2
        );

    // Iterates over cells and registers points index in index buffer.
    const auto cell_type = _mesh->get_cell_type();

    // TODO: The loop before is probably not optimal and would need some extra optimization work
    // See comments in https://git.ircad.fr/sight/sight/-/merge_requests/476
    unsigned int count_index = 0;

    if(cell_type == sight::data::mesh::cell_type_t::TRIANGLE)
    {
        for(const auto& cell : _mesh->crange<cell::triangle>())
        {
            raw_index_buffer_data[count_index++] = cell.pt[0];
            raw_index_buffer_data[count_index++] = cell.pt[1];
            raw_index_buffer_data[count_index++] = cell.pt[2];
        }
    }
    else if(cell_type == sight::data::mesh::cell_type_t::QUAD)
    {
        // Resizes index buffer if quad mesh to have correct number of indices once quad mesh is converted to
        // triangle mesh.
        index_buffer_data.resize(
            6 * static_cast<int>(_mesh->numCells())
            * static_cast<int>(sizeof(unsigned int))
        );
        raw_index_buffer_data = reinterpret_cast<unsigned int*>(index_buffer_data.data());

        m_indexAttrib->setCount(6 * static_cast<unsigned int>(_mesh->numCells()));

        this->addComputeEntityToScene(static_cast<int>(_mesh->numCells()));
        for(const auto& cell : _mesh->crange<cell::quad>())
        {
            raw_index_buffer_data[count_index++] = cell.pt[0];
            raw_index_buffer_data[count_index++] = cell.pt[1];
            raw_index_buffer_data[count_index++] = cell.pt[2];
            raw_index_buffer_data[count_index++] = cell.pt[3];

            raw_index_buffer_data[count_index++]     = 0;
            raw_index_buffer_data[count_index++ + 1] = cell.pt[0];
            raw_index_buffer_data[count_index++ + 2] = 0;
        }
    }

    // Associates QBuffers with respective data arrays.
    m_posBuffer->setData(pos_buffer_data);
    m_normalBuffer->setData(normal_buffer_data);
    m_indexBuffer->setData(index_buffer_data);
}

//------------------------------------------------------------------------------

void mesh::addComputeEntityToScene(int _number_of_cells)
{
    auto* const compute_entity  = new Qt3DCore::QEntity(m_scene);
    auto* const compute_command = new Qt3DRender::QComputeCommand(compute_entity);

    auto* const compute_mat = new viz::qt3d::ComputeMaterial();
    compute_mat->setIndexBuffer(m_indexBuffer);

    compute_entity->addComponent(compute_mat);
    compute_entity->addComponent(compute_command);

    compute_mat->updateFrameGraph(m_scene->getFrameGraph(), _number_of_cells);
}

} // namespace sight::viz::qt3d::data
