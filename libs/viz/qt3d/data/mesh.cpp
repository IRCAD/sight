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
#include "viz/qt3d/core/generic_scene.hpp"

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
    m_scene(qobject_cast<sight::viz::qt3d::core::generic_scene*>(_parent)),
    m_geom_renderer(new Qt3DRender::QGeometryRenderer(m_scene)),
    m_geometry(new Qt3DRender::QGeometry(m_geom_renderer)),
    m_material(new viz::qt3d::data::material()),
    m_pos_attrib(new Qt3DRender::QAttribute(m_geometry)),
    m_normal_attrib(new Qt3DRender::QAttribute(m_geometry)),
    m_index_attrib(new Qt3DRender::QAttribute(m_geometry)),
    m_pos_buffer(new Qt3DRender::QBuffer(m_geometry)),
    m_normal_buffer(new Qt3DRender::QBuffer(m_geometry)),
    m_index_buffer(new Qt3DRender::QBuffer(m_geometry))
{
    this->addComponent(m_geom_renderer);
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

mesh::~mesh()
= default;

//------------------------------------------------------------------------------

viz::qt3d::data::material* mesh::get_material() const
{
    return m_material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::generic_scene* mesh::get_scene() const
{
    return m_scene;
}

//------------------------------------------------------------------------------

void mesh::set_material(viz::qt3d::data::material* _material)
{
    SIGHT_ASSERT("Material can't be set to null value.", _material);
    this->removeComponent(m_material);
    m_material = _material;
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

void mesh::set_scene(sight::viz::qt3d::core::generic_scene* _scene)
{
    SIGHT_ASSERT("scene can't be set to null value.", _scene);
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void mesh::set_mesh(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Sets the number of points (for a mesh of triangles).
    m_number_of_points = static_cast<unsigned int>(_mesh->num_points());

    if(!m_geometry->attributes().empty())
    {
        m_geometry->removeAttribute(m_pos_attrib);
        m_geometry->removeAttribute(m_normal_attrib);
        m_geometry->removeAttribute(m_index_attrib);
    }

    // Configures rendering attributes.
    m_pos_attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_pos_attrib->setBuffer(m_pos_buffer);
    m_pos_attrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_pos_attrib->setVertexSize(m_vertex_size);
    m_pos_attrib->setByteOffset(0);
    m_pos_attrib->setByteStride(3 * sizeof(float));
    m_pos_attrib->setCount(m_number_of_points);
    m_pos_attrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    m_geometry->addAttribute(m_pos_attrib);

    m_normal_attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normal_attrib->setBuffer(m_normal_buffer);
    m_normal_attrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_normal_attrib->setVertexSize(m_vertex_size);
    m_normal_attrib->setByteOffset(0);
    m_normal_attrib->setByteStride(3 * sizeof(float));
    m_normal_attrib->setCount(m_number_of_points);
    m_normal_attrib->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    m_geometry->addAttribute(m_normal_attrib);

    m_index_attrib->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    m_index_attrib->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    m_index_attrib->setBuffer(m_index_buffer);
    m_index_attrib->setCount(3 * static_cast<unsigned int>(_mesh->num_cells()));
    m_index_attrib->setName("indexAttribute");

    m_geometry->addAttribute(m_index_attrib);

    // Fills buffers with data extracted from _mesh.
    this->build_buffers(_mesh);

    m_geometry->setBoundingVolumePositionAttribute(m_pos_attrib);

    m_geom_renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_geom_renderer->setGeometry(m_geometry);
}

//------------------------------------------------------------------------------

void mesh::center_camera_on_mesh()
{
    auto* camera = m_scene->get_camera();
    camera->viewSphere(
        m_mesh_center,
        std::max(
            std::max(m_max_extent.x() - m_min_extent.x(), m_max_extent.y() - m_min_extent.y()),
            m_max_extent.z() - m_min_extent.z()
        )
    );
}

//------------------------------------------------------------------------------

void mesh::build_buffers(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Returns if _mesh is empty.
    if(static_cast<int>(_mesh->num_cells()) == 0 || static_cast<int>(_mesh->num_points()) == 0)
    {
        return;
    }

    // Declares data arrays which are associated with QBuffers.
    QByteArray pos_buffer_data;
    pos_buffer_data.resize(
        static_cast<int>(static_cast<std::size_t>(m_number_of_points) * m_vertex_size * sizeof(float))
    );
    auto* raw_pos_buffer_data = reinterpret_cast<float*>(pos_buffer_data.data());

    QByteArray normal_buffer_data;
    normal_buffer_data.resize(static_cast<int>(std::size_t(m_number_of_points) * m_vertex_size * sizeof(float)));
    auto* raw_normal_buffer_data = reinterpret_cast<float*>(normal_buffer_data.data());

    QByteArray index_buffer_data;
    index_buffer_data.resize(3 * static_cast<int>(_mesh->num_cells()) * static_cast<int>(sizeof(unsigned int)));
    auto* raw_index_buffer_data = reinterpret_cast<unsigned int*>(index_buffer_data.data());

    // Checks if the mesh has normals. If not, generates them.
    if(!_mesh->has<sight::data::mesh::attribute::point_normals>())
    {
        geometry::data::mesh::generate_point_normals(_mesh);
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

    m_min_extent = QVector3D(min_x, min_y, min_z);
    m_max_extent = QVector3D(max_x, max_y, max_z);

    m_mesh_center =
        QVector3D(
            (m_min_extent.x() + m_max_extent.x()) / 2,
            (m_min_extent.y() + m_max_extent.y()) / 2,
            (m_min_extent.z() + m_max_extent.z()) / 2
        );

    // Iterates over cells and registers points index in index buffer.
    const auto cell_type = _mesh->cell_type();

    // TODO: The loop before is probably not optimal and would need some extra optimization work
    // See comments in https://git.ircad.fr/sight/sight/-/merge_requests/476
    unsigned int count_index = 0;

    if(cell_type == sight::data::mesh::cell_type_t::triangle)
    {
        for(const auto& cell : _mesh->crange<cell::triangle>())
        {
            raw_index_buffer_data[count_index++] = cell.pt[0];
            raw_index_buffer_data[count_index++] = cell.pt[1];
            raw_index_buffer_data[count_index++] = cell.pt[2];
        }
    }
    else if(cell_type == sight::data::mesh::cell_type_t::quad)
    {
        // Resizes index buffer if quad mesh to have correct number of indices once quad mesh is converted to
        // triangle mesh.
        index_buffer_data.resize(
            6 * static_cast<int>(_mesh->num_cells())
            * static_cast<int>(sizeof(unsigned int))
        );
        raw_index_buffer_data = reinterpret_cast<unsigned int*>(index_buffer_data.data());

        m_index_attrib->setCount(6 * static_cast<unsigned int>(_mesh->num_cells()));

        this->add_compute_entity_to_scene(static_cast<int>(_mesh->num_cells()));
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
    m_pos_buffer->setData(pos_buffer_data);
    m_normal_buffer->setData(normal_buffer_data);
    m_index_buffer->setData(index_buffer_data);
}

//------------------------------------------------------------------------------

void mesh::add_compute_entity_to_scene(int _number_of_cells)
{
    auto* const compute_entity  = new Qt3DCore::QEntity(m_scene);
    auto* const compute_command = new Qt3DRender::QComputeCommand(compute_entity);

    auto* const compute_mat = new viz::qt3d::compute_material();
    compute_mat->set_index_buffer(m_index_buffer);

    compute_entity->addComponent(compute_mat);
    compute_entity->addComponent(compute_command);

    compute_mat->update_frame_graph(m_scene->get_frame_graph(), _number_of_cells);
}

} // namespace sight::viz::qt3d::data
