/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "viz/scene3d/mesh.hpp"

#include "viz/scene3d/r2vb_renderable.hpp"
#include <viz/scene3d/helper/mesh.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/data/mesh.hpp>

#include <OgreEntity.h>
#include <OgreHardwareBufferManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreSubMesh.h>
#include <OgreTextureManager.h>

#include <cmath>
#include <ranges>

namespace sight::viz::scene3d
{

const unsigned int mesh::MAX_TEXTURE_SIZE;

using attribute = data::mesh::attribute;

//-----------------------------------------------------------------------------

template<typename T>
void copy_indices(
    void* _cells,
    const data::mesh& _mesh
)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* cells = static_cast<T*>(_cells);

    const auto type = _mesh.cell_type();

    if(type == data::mesh::cell_type_t::triangle)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::triangle>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
        }
    }
    else if(type == data::mesh::cell_type_t::quad)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::quad>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
            *cells++ = static_cast<T>(cell.pt[3]);
        }
    }
    else if(type == data::mesh::cell_type_t::line)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::line>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
        }
    }
    else if(type == data::mesh::cell_type_t::tetra)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::tetra>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
            *cells++ = static_cast<T>(cell.pt[3]);
        }
    }
}

//------------------------------------------------------------------------------

mesh::mesh(const std::string& _name)
{
    m_binding[position_normal] = 0;
    m_binding[colour]          = 0xFFFF;
    m_binding[texcoord]        = 0xFFFF;

    auto& mesh_mgr = Ogre::MeshManager::getSingleton();

    // Create mesh Data Structure
    const auto mesh_name = _name + "_Mesh";
    m_ogre_mesh = mesh_mgr.createManual(mesh_name, viz::scene3d::RESOURCE_GROUP);

    // Create mesh Data Structure for r2vb input
    // We could create it only when necessary, but for now it is simpler to create it every time
    const auto r2vb_mesh_name = _name + "_R2VBMesh";
    m_r2vb_mesh = mesh_mgr.createManual(
        r2vb_mesh_name,
        viz::scene3d::RESOURCE_GROUP
    );

    // TODO: remove ?
    m_per_primitive_color_texture.reset();
}

//------------------------------------------------------------------------------

mesh::~mesh()
{
    // Destroy Ogre mesh
    auto& mesh_mgr = Ogre::MeshManager::getSingleton();
    mesh_mgr.remove(m_ogre_mesh->getHandle());
    mesh_mgr.remove(m_r2vb_mesh->getHandle());
    m_ogre_mesh.reset();
    m_r2vb_mesh.reset();
}

//-----------------------------------------------------------------------------

void mesh::bind_layer(
    const data::mesh::csptr& _mesh,
    buffer_binding _binding,
    Ogre::VertexElementSemantic _semantic,
    Ogre::VertexElementType _type
)
{
    Ogre::VertexBufferBinding* bind = m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    Ogre::VertexDeclaration* vtx_decl = m_ogre_mesh->sharedVertexData->vertexDeclaration;

    // Create the buffer semantic if it does not exist.
    if(vtx_decl->findElementBySemantic(_semantic) == nullptr)
    {
        m_binding[_binding] = static_cast<std::uint16_t>(bind->getBindings().size());

        vtx_decl->addElement(m_binding[_binding], 0, _type, _semantic);
    }

    // Get requested buffer size and previous buffer size.
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer;

    const std::size_t ui_num_vertices = _mesh->num_points();
    std::size_t ui_prev_num_vertices  = 0;
    if(bind->isBufferBound(m_binding[_binding]))
    {
        vertex_buffer        = bind->getBuffer(m_binding[_binding]);
        ui_prev_num_vertices = vertex_buffer->getNumVertices();
    }

    // Allocate the buffer if it necessary.
    if(!bind->isBufferBound(m_binding[_binding]) || ui_prev_num_vertices < ui_num_vertices)
    {
        FW_PROFILE_AVG("REALLOC LAYER", 5);

        // Allocate color buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
        auto usage =
            (m_is_dynamic || m_is_dynamic_vertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        const std::size_t offset = Ogre::VertexElement::getTypeSize(_type);

        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, ui_num_vertices, usage, false);
        bind->setBinding(m_binding[_binding], vertex_buffer);
    }
}

//------------------------------------------------------------------------------

void mesh::set_visible(bool _visible)
{
    if(m_r2vb_entity != nullptr)
    {
        m_r2vb_entity->setVisible(_visible);
    }

    for(auto& val : m_r2vb_object | std::views::values)
    {
        val->setVisible(_visible);
    }
}

//------------------------------------------------------------------------------

void mesh::update_mesh(const data::mesh::csptr& _mesh, bool _points_only)
{
    const auto dump_lock = _mesh->dump_lock();

    /// The values in this table refer to vertices in the above table
    const std::size_t num_vertices = _mesh->num_points();
    SIGHT_DEBUG("Vertices count: " << num_vertices);

    // Check if the mesh has normals - we assume we should have as many normals as points
    // If this is not the case, normals will be ignored or regenerated if needed and if the number of vertices changed
    m_layout = m_layout | (_mesh->attributes() & attribute::point_normals);

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(m_ogre_mesh->sharedVertexData == nullptr)
    {
        m_ogre_mesh->sharedVertexData = new Ogre::VertexData();
    }

    Ogre::VertexBufferBinding& bind = *m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    std::size_t prev_num_vertices   = 0;
    if(bind.isBufferBound(m_binding[position_normal]))
    {
        prev_num_vertices = bind.getBuffer(m_binding[position_normal])->getNumVertices();
    }

    if(!data::mesh::has<attribute::point_normals>(m_layout) && !_points_only)
    {
        // Verify if mesh contains Tetra, Edge or Point
        // If not, generate normals

        const auto cell_type       = _mesh->cell_type();
        const bool compute_normals = (cell_type != data::mesh::cell_type_t::line
                                      && cell_type != data::mesh::cell_type_t::tetra
                                      && cell_type != data::mesh::cell_type_t::point);

        if(compute_normals)
        {
            // /!\ DEPRECATED /!\: normals shouldn't be computed by an adaptor.
            // We need to remove the const of the _mesh to compute normals.
            geometry::data::mesh::generate_point_normals(std::const_pointer_cast<data::mesh>(_mesh));
            m_layout = m_layout | attribute::point_normals;
        }
    }

    if(prev_num_vertices < num_vertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogre_mesh->sharedVertexData->vertexCount = Ogre::uint32(num_vertices);

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        auto usage =
            (m_is_dynamic || m_is_dynamic_vertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::mesh
        Ogre::VertexDeclaration* decl_main = m_ogre_mesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        decl_main->removeAllElements();

        // 1st buffer
        decl_main->addElement(m_binding[position_normal], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        if(data::mesh::has<attribute::point_normals>(m_layout))
        {
            decl_main->addElement(m_binding[position_normal], offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
            offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
        }

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, num_vertices, usage, false);
        bind.setBinding(m_binding[position_normal], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogre_mesh->sharedVertexData->vertexCount = Ogre::uint32(num_vertices);
    }

    //------------------------------------------
    // Create indices arrays
    //------------------------------------------

    // 1 - Create a submesh for the primitive type
    const bool indices32_bits      = num_vertices >= (1 << 16);
    const bool indices_prev32_bits = prev_num_vertices >= (1 << 16);
    const bool has_primitive_color = _mesh->has<attribute::cell_colors>();
    {
        FW_PROFILE_AVG("REALLOC INDEX", 5);

        const data::mesh::cell_type_t cell_type      = _mesh->cell_type();
        const data::mesh::cell_type_t prev_cell_type = m_cell_type;

        const bool destroy_mesh = _mesh->num_cells() == 0 || prev_cell_type != cell_type
                                  || data::mesh::has<attribute::cell_colors>(m_layout) != has_primitive_color;

        // Destroy the submesh if it has been created before - a submesh with 0 index would be invalid
        if(destroy_mesh && (m_sub_mesh != nullptr))
        {
            std::string name = std::to_string(static_cast<int>(prev_cell_type));
            if((prev_cell_type == data::mesh::cell_type_t::triangle && has_primitive_color)
               || (prev_cell_type == data::mesh::cell_type_t::tetra || prev_cell_type == data::mesh::cell_type_t::quad))
            {
                m_r2vb_mesh->destroySubMesh(name);
            }
            else
            {
                m_ogre_mesh->destroySubMesh(name);
            }

            m_sub_mesh = nullptr;
        }

        if(_mesh->num_cells() > 0)
        {
            if(m_sub_mesh == nullptr)
            {
                // Create one submesh
                const std::string name = std::to_string(static_cast<int>(cell_type));
                if(cell_type == data::mesh::cell_type_t::triangle)
                {
                    if(has_primitive_color)
                    {
                        // Use r2vb pipeline for per-primitive color
                        m_sub_mesh = m_r2vb_mesh->createSubMesh(name);
                    }
                    else
                    {
                        m_sub_mesh = m_ogre_mesh->createSubMesh(name);
                    }

                    m_sub_mesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
                }
                else if(cell_type == data::mesh::cell_type_t::line)
                {
                    m_sub_mesh                = m_ogre_mesh->createSubMesh(name);
                    m_sub_mesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
                }
                else if(cell_type == data::mesh::cell_type_t::point)
                {
                    m_sub_mesh                = m_ogre_mesh->createSubMesh(name);
                    m_sub_mesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
                }
                else if(cell_type == data::mesh::cell_type_t::quad || cell_type == data::mesh::cell_type_t::tetra)
                {
                    // Use r2vb pipeline to generate quads or tetrahedrons
                    m_sub_mesh                = m_r2vb_mesh->createSubMesh(name);
                    m_sub_mesh->operationType = Ogre::RenderOperation::OT_LINE_LIST_ADJ;
                }
                else
                {
                    SIGHT_ASSERT("invalid cell type", false);
                }

                m_sub_mesh->useSharedVertices     = true;
                m_sub_mesh->indexData->indexStart = 0;
            }

            if(cell_type != data::mesh::cell_type_t::point)
            {
                Ogre::HardwareIndexBufferSharedPtr ibuf = m_sub_mesh->indexData->indexBuffer;
                const unsigned int num_indices          = _mesh->num_cells() * _mesh->cell_size();

                // Allocate index buffer of the requested number of vertices (ibufCount) if necessary
                // We don't reallocate if we have more space than requested
                bool create_index_buffer = !ibuf;
                if(ibuf)
                {
                    // reallocate if new mesh has more indexes or index_t change
                    create_index_buffer = (ibuf->getNumIndexes() < num_indices
                                           || (indices_prev32_bits != indices32_bits));
                }

                if(create_index_buffer)
                {
                    auto usage =
                        m_is_dynamic
                        ? static_cast<Ogre::HardwareBuffer::Usage>(
                            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                        )
                        : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

                    ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                        indices32_bits ? Ogre::HardwareIndexBuffer::IT_32BIT : Ogre::HardwareIndexBuffer::IT_16BIT,
                        num_indices,
                        usage
                    );

                    m_sub_mesh->indexData->indexBuffer = ibuf;
                }

                m_sub_mesh->indexData->indexCount = num_indices;
                SIGHT_DEBUG("Index #" << m_sub_mesh->indexData->indexCount);

                // Lock index data, we are going to write into it in the next loop
                void* index_buffer = ibuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);

                if(indices32_bits)
                {
                    copy_indices<std::uint32_t>(index_buffer, *_mesh);
                }
                else
                {
                    copy_indices<std::uint16_t>(index_buffer, *_mesh);
                }
            }
        }

        if(m_sub_mesh != nullptr && m_sub_mesh->indexData->indexBuffer)
        {
            m_sub_mesh->indexData->indexBuffer->unlock();
        }

        m_cell_type = cell_type;
    }
}

//------------------------------------------------------------------------------

void mesh::update_mesh(const data::point_list::csptr& _point_list)
{
    auto points = _point_list->get_points();

    /// The values in this table refer to vertices in the above table
    std::size_t ui_num_vertices = points.size();
    SIGHT_DEBUG("Vertices #" << ui_num_vertices);

    m_layout = m_layout & ~attribute::point_normals;

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(m_ogre_mesh->sharedVertexData == nullptr)
    {
        m_ogre_mesh->sharedVertexData = new Ogre::VertexData();
    }

    Ogre::VertexBufferBinding& bind  = *m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    std::size_t ui_prev_num_vertices = 0;
    if(bind.isBufferBound(m_binding[position_normal]))
    {
        ui_prev_num_vertices = bind.getBuffer(m_binding[position_normal])->getNumVertices();
    }

    if(ui_prev_num_vertices < ui_num_vertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogre_mesh->sharedVertexData->vertexCount = Ogre::uint32(ui_num_vertices);

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        auto usage =
            (m_is_dynamic || m_is_dynamic_vertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::mesh
        Ogre::VertexDeclaration* decl_main = m_ogre_mesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        decl_main->removeAllElements();

        // 1st buffer
        decl_main->addElement(m_binding[position_normal], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, ui_num_vertices, usage, false);
        bind.setBinding(m_binding[position_normal], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogre_mesh->sharedVertexData->vertexCount = Ogre::uint32(ui_num_vertices);
    }

    if(m_sub_mesh == nullptr)
    {
        const auto point_type = static_cast<std::size_t>(data::mesh::cell_type_t::point);
        m_cell_type               = data::mesh::cell_type_t::point;
        m_sub_mesh                = m_ogre_mesh->createSubMesh(std::to_string(point_type));
        m_sub_mesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::vector<r2vb_renderable*> > mesh::update_r2vb(
    const data::mesh::csptr& _mesh,
    Ogre::SceneManager& _scene_mgr,
    const std::string& _material_name
)
{
    //------------------------------------------
    // Render to vertex-buffer
    // Three cases to handle:
    // - Quads primitives generation - 2 triangles from 4 points
    // - Tetrahedrons primitives generation - 4 triangles from 4 points
    // - Per-primitive color generation - either triangles, quads or tetrahedrons
    //------------------------------------------
    std::vector<r2vb_renderable*> r2vb_renderables;
    bool add = true;

    const bool has_primitive_color = _mesh->has<attribute::cell_colors>();
    const bool b_quad              = m_cell_type == data::mesh::cell_type_t::quad;
    const bool b_tetra             = m_cell_type == data::mesh::cell_type_t::tetra;
    if(b_quad || b_tetra || has_primitive_color)
    {
        if(m_r2vb_mesh->sharedVertexData == nullptr)
        {
            m_r2vb_mesh->sharedVertexData = new Ogre::VertexData();
        }

        *m_r2vb_mesh->sharedVertexData->vertexBufferBinding = *m_ogre_mesh->sharedVertexData->vertexBufferBinding;
        *m_r2vb_mesh->sharedVertexData->vertexDeclaration   = *m_ogre_mesh->sharedVertexData->vertexDeclaration;
        m_r2vb_mesh->sharedVertexData->vertexCount          = m_ogre_mesh->sharedVertexData->vertexCount;

        m_r2vb_mesh->_setBounds(m_ogre_mesh->getBounds());
        m_r2vb_mesh->_setBoundingSphereRadius(m_ogre_mesh->getBoundingSphereRadius());

        // Add mesh to Ogre Root scene Node if it doesn't exist yet
        if(m_r2vb_entity == nullptr)
        {
            m_r2vb_entity = _scene_mgr.createEntity(m_r2vb_mesh);
        }

        const std::size_t num_sub_entities = m_r2vb_entity->getNumSubEntities();
        for(std::size_t i = 0 ; i < num_sub_entities ; ++i)
        {
            auto* const sub_entity = m_r2vb_entity->getSubEntity(i);
            auto* const sub_mesh   = sub_entity->getSubMesh();

            const data::mesh::cell_type_t cell_type = b_quad ? data::mesh::cell_type_t::quad
                                                             : b_tetra ? data::mesh::cell_type_t::tetra
                                                                       : data::mesh::cell_type_t::triangle;

            if(m_r2vb_object.find(cell_type) == m_r2vb_object.end())
            {
                const std::string name             = std::to_string(static_cast<int>(cell_type));
                const std::string r2vb_object_name = m_ogre_mesh->getName() + "_r2vbObject_" + name;
                m_r2vb_object[cell_type] = viz::scene3d::r2vb_renderable::make(
                    r2vb_object_name,
                    sub_entity,
                    &_scene_mgr,
                    cell_type,
                    _material_name
                );
            }

            m_r2vb_object[cell_type]->set_output_settings(
                static_cast<unsigned int>(sub_mesh->indexData->indexCount),
                data::mesh::has<attribute::cell_colors>(m_layout) || data::mesh::has<attribute::point_colors>(m_layout),
                data::mesh::has<attribute::point_tex_coords>(m_layout)
            );

            r2vb_renderables.push_back(m_r2vb_object[cell_type]);
        }

        add = true;
    }
    else
    {
        // Clear if necessary
        for(auto* val : m_r2vb_object | std::views::values)
        {
            r2vb_renderables.push_back(val);
        }

        m_r2vb_object.clear();

        if(m_r2vb_entity != nullptr)
        {
            _scene_mgr.destroyEntity(m_r2vb_entity);
            m_r2vb_entity = nullptr;
        }

        add = false;
    }

    return std::make_pair(add, r2vb_renderables);
}

//-----------------------------------------------------------------------------

void mesh::update_vertices(const data::mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[position_normal]);

    /// Upload the vertex data to the GPU
    void* p_vertex = vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre mesh with data::mesh
    const auto dump_lock = _mesh->dump_lock();

    std::size_t ui_stride_float = 3;
    if(data::mesh::has<attribute::point_normals>(m_layout))
    {
        ui_stride_float += 3;
    }

    using position_t = data::mesh::position_t;
    using normal_t   = data::mesh::normal_t;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    position_t x_min = std::numeric_limits<position_t>::max();
    position_t y_min = std::numeric_limits<position_t>::max();
    position_t z_min = std::numeric_limits<position_t>::max();
    position_t x_max = std::numeric_limits<position_t>::lowest();
    position_t y_max = std::numeric_limits<position_t>::lowest();
    position_t z_max = std::numeric_limits<position_t>::lowest();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(const auto& p : _mesh->crange<data::iterator::point::xyz>())
        {
            const auto& pt0 = p.x;
            x_min = std::min(x_min, pt0);
            x_max = std::max(x_max, pt0);

            const auto& pt1 = p.y;
            y_min = std::min(y_min, pt1);
            y_max = std::max(y_max, pt1);

            const auto& pt2 = p.z;
            z_min = std::min(z_min, pt2);
            z_max = std::max(z_max, pt2);
        }
    }
    {
        FW_PROFILE_AVG("UPDATE POS AND NORMALS", 5);
        auto* __restrict p_pos = static_cast<position_t*>(p_vertex);
        for(const auto& p : _mesh->crange<data::iterator::point::xyz>())
        {
            memcpy(p_pos, &p.x, 3 * sizeof(position_t));
            p_pos += ui_stride_float;
        }

        normal_t* __restrict p_normal = nullptr;

        if(data::mesh::has<attribute::point_normals>(m_layout))
        {
            p_normal = static_cast<normal_t*>(p_vertex) + 3;

            for(const auto& n : _mesh->crange<data::iterator::point::nxyz>())
            {
                memcpy(p_normal, &n.nx, 3 * sizeof(normal_t));
                p_normal += ui_stride_float;
            }
        }
    }

    // Unlock vertex data
    vertex_buffer->unlock();

    if(x_min < std::numeric_limits<position_t>::max()
       && y_min < std::numeric_limits<position_t>::max()
       && z_min < std::numeric_limits<position_t>::max()
       && x_max > std::numeric_limits<position_t>::lowest()
       && y_max > std::numeric_limits<position_t>::lowest()
       && z_max > std::numeric_limits<position_t>::lowest())
    {
        m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox(x_min, y_min, z_min, x_max, y_max, z_max));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::mesh::are_bounds_valid(m_ogre_mesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            m_ogre_mesh->_setBoundingSphereRadius(
                Ogre::Math::Sqrt(
                    Ogre::Math::Sqr(x_max - x_min)
                    + Ogre::Math::Sqr(y_max - y_min)
                    + Ogre::Math::Sqr(z_max - z_min)
                ) / 2
            );
        }
        else
        {
            SIGHT_ERROR("Infinite or NaN values for the bounding box. Check the mesh validity.");

            // This silent the problem so there is no crash in Ogre
            m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify mesh object that it has been modified
    m_ogre_mesh->load();
}

//-----------------------------------------------------------------------------

void mesh::update_vertices(const data::point_list::csptr& _point_list)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[position_normal]);

    /// Upload the vertex data to the GPU
    void* p_vertex = vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre mesh with data::mesh
    const std::size_t ui_stride_float = 3;

    using point_t = data::point::point_coord_t;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    point_t x_min = std::numeric_limits<point_t>::max();
    point_t y_min = std::numeric_limits<point_t>::max();
    point_t z_min = std::numeric_limits<point_t>::max();
    point_t x_max = std::numeric_limits<point_t>::lowest();
    point_t y_max = std::numeric_limits<point_t>::lowest();
    point_t z_max = std::numeric_limits<point_t>::lowest();

    const auto& points           = _point_list->get_points();
    const std::size_t num_points = points.size();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            const auto point = points[i]->get_coord();
            const auto& pt0  = point[0];
            x_min = std::min(x_min, pt0);
            x_max = std::max(x_max, pt0);

            const auto& pt1 = point[1];
            y_min = std::min(y_min, pt1);
            y_max = std::max(y_max, pt1);

            const auto& pt2 = point[2];
            z_min = std::min(z_min, pt2);
            z_max = std::max(z_max, pt2);
        }
    }
    {
        auto* __restrict p_pos = static_cast<float*>(p_vertex);
        FW_PROFILE_AVG("UPDATE POS", 5);
        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            const auto point = points[i]->get_coord();
            p_pos[0] = static_cast<float>(point[0]);
            p_pos[1] = static_cast<float>(point[1]);
            p_pos[2] = static_cast<float>(point[2]);

            p_pos += ui_stride_float;
        }
    }
    // Unlock vertex data
    vertex_buffer->unlock();

    if(x_min < std::numeric_limits<point_t>::max()
       && y_min < std::numeric_limits<point_t>::max()
       && z_min < std::numeric_limits<point_t>::max()
       && x_max > std::numeric_limits<point_t>::lowest()
       && y_max > std::numeric_limits<point_t>::lowest()
       && z_max > std::numeric_limits<point_t>::lowest())
    {
        const auto x_min_f = static_cast<float>(x_min);
        const auto y_min_f = static_cast<float>(y_min);
        const auto z_min_f = static_cast<float>(z_min);
        const auto x_max_f = static_cast<float>(x_max);
        const auto y_max_f = static_cast<float>(y_max);
        const auto z_max_f = static_cast<float>(z_max);

        m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox(x_min_f, y_min_f, z_min_f, x_max_f, y_max_f, z_max_f));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::mesh::are_bounds_valid(m_ogre_mesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            const float x_len_f = x_max_f - x_min_f;
            const float y_len_f = y_max_f - y_min_f;
            const float z_len_f = z_max_f - x_min_f;

            m_ogre_mesh->_setBoundingSphereRadius(
                Ogre::Math::Sqrt(
                    Ogre::Math::Sqr(x_len_f)
                    + Ogre::Math::Sqr(y_len_f)
                    + Ogre::Math::Sqr(z_len_f)
                ) / 2.0F
            );
        }
        else
        {
            SIGHT_ERROR("Infinite or NaN values for the bounding box. Check the mesh validity.");

            // This silent the problem so there is no crash in Ogre
            m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogre_mesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify mesh object that it has been modified
    m_ogre_mesh->load();
}

//-----------------------------------------------------------------------------

void mesh::update_colors(const data::mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    Ogre::VertexBufferBinding* bind = m_ogre_mesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    const bool has_vertex_color    = _mesh->has<attribute::point_colors>();
    const bool has_primitive_color = _mesh->has<attribute::cell_colors>();

    // 1 - Initialization
    if(has_vertex_color)
    {
        bind_layer(_mesh, colour, Ogre::VES_DIFFUSE, Ogre::VET_COLOUR);
    }
    else
    {
        // Unbind vertex color if it was previously enabled
        if(bind->isBufferBound(m_binding[colour]))
        {
            bind->unsetBinding(m_binding[colour]);
            m_binding[colour] = 0xFFFF;
        }
    }

    if(has_primitive_color)
    {
        unsigned int num_indices_total = 0;

        if(m_sub_mesh != nullptr)
        {
            if(_mesh->cell_type() == data::mesh::cell_type_t::triangle)
            {
                num_indices_total += static_cast<unsigned int>(m_sub_mesh->indexData->indexCount / 3);
            }
            else
            {
                num_indices_total += static_cast<unsigned int>(m_sub_mesh->indexData->indexCount >> 2);
            }
        }

        if(!m_per_primitive_color_texture)
        {
            static std::uint64_t i = 0;
            m_per_primitive_color_texture_name = "PerCellColorTexture_" + std::to_string(i++);
            m_per_primitive_color_texture      = Ogre::TextureManager::getSingleton().create(
                m_per_primitive_color_texture_name,
                viz::scene3d::RESOURCE_GROUP,
                true
            );
        }

        const std::size_t width = std::min(MAX_TEXTURE_SIZE, num_indices_total);
        const auto height       = static_cast<std::size_t>(std::floor(num_indices_total / MAX_TEXTURE_SIZE) + 1);

        if(m_per_primitive_color_texture->getWidth() != width || m_per_primitive_color_texture->getHeight() != height)
        {
            FW_PROFILE_AVG("REALLOC COLORS_CELL", 5);

            // It would be better to use PF_BYTE_RGB when we have 3 components but for some reason it doesn't work
            // Probably something related to alignment or a bug in Ogre
            viz::scene3d::utils::allocate_texture(
                m_per_primitive_color_texture.get(),
                width,
                height,
                1,
                Ogre::PF_BYTE_RGBA,
                Ogre::TEX_TYPE_2D,
                (m_is_dynamic || m_is_dynamic_vertices)
            );
        }
    }
    else
    {
        if(m_per_primitive_color_texture)
        {
            m_per_primitive_color_texture->unload();
        }

        m_per_primitive_color_texture.reset();
        m_per_primitive_color_texture_name = "";
    }

    const auto lock = _mesh->dump_lock();
    // 2 - Copy of vertices
    if(has_vertex_color)
    {
        // Source points
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[colour]);
        auto* p_color                                     =
            static_cast<Ogre::RGBA*>(vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::point::rgba>())->r;

        // Copy points
        const std::size_t nb_components = 4;
        viz::scene3d::helper::mesh::copy_colors(p_color, colors, _mesh->num_points(), nb_components);

        vertex_buffer->unlock();
    }

    if(has_primitive_color)
    {
        // Source cells
        Ogre::HardwarePixelBufferSharedPtr pixel_buffer = m_per_primitive_color_texture->getBuffer();
        pixel_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixel_box = pixel_buffer->getCurrentLock();
        auto* p_color_dest              = reinterpret_cast<Ogre::RGBA*>(pixel_box.data);

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::cell::rgba>())->r;

        // Copy cells
        const std::size_t nb_components = 4;
        viz::scene3d::helper::mesh::copy_colors(p_color_dest, colors, _mesh->num_cells(), nb_components);

        pixel_buffer->unlock();
    }

    if(has_vertex_color != data::mesh::has<attribute::point_colors>(m_layout) || has_primitive_color != data::mesh::has<attribute::cell_colors>(m_layout))
    {
        m_layout = m_layout | (_mesh->attributes() & attribute::point_colors);
        m_layout = m_layout | (_mesh->attributes() & attribute::cell_colors);
    }

    /// Notify mesh object that it has been modified
    m_ogre_mesh->load();
}

//-----------------------------------------------------------------------------

void mesh::update_tex_coords(const data::mesh::csptr& _mesh)
{
    m_layout = m_layout | (_mesh->attributes() & attribute::point_tex_coords);

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if(data::mesh::has<attribute::point_tex_coords>(m_layout))
    {
        bind_layer(_mesh, texcoord, Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        Ogre::VertexBufferBinding* bind               = m_ogre_mesh->sharedVertexData->vertexBufferBinding;
        Ogre::HardwareVertexBufferSharedPtr uv_buffer = bind->getBuffer(m_binding[texcoord]);
        void* p_buf                                   = uv_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        auto* p_uv                                    = static_cast<float*>(p_buf);

        // Copy UV coordinates for each mesh point
        for(const auto& uv : _mesh->crange<data::iterator::point::uv>())
        {
            p_uv[0] = uv.u;
            p_uv[1] = uv.v;
            p_uv   += 2;
        }

        uv_buffer->unlock();
    }

    /// Notify mesh object that it has been modified
    m_ogre_mesh->load();
}

//-----------------------------------------------------------------------------

void mesh::clear_mesh(Ogre::SceneManager& _scene_mgr)
{
    // Destroy all the submeshes, but keep the two meshes alive

    if(m_sub_mesh != nullptr)
    {
        const data::mesh::cell_type_t cell_type = m_cell_type;
        const std::string name                  = std::to_string(static_cast<int>(cell_type));
        if((cell_type == data::mesh::cell_type_t::triangle && data::mesh::has<attribute::cell_colors>(m_layout))
           || (cell_type == data::mesh::cell_type_t::tetra || cell_type == data::mesh::cell_type_t::quad))
        {
            m_r2vb_mesh->destroySubMesh(name);
        }
        else
        {
            m_ogre_mesh->destroySubMesh(name);
        }

        m_sub_mesh = nullptr;
    }

    for(auto r2vb_object : m_r2vb_object)
    {
        _scene_mgr.destroyMovableObject(r2vb_object.second);
    }

    m_r2vb_object.clear();

    if(m_r2vb_entity != nullptr)
    {
        _scene_mgr.destroyEntity(m_r2vb_entity);
        m_r2vb_entity = nullptr;
    }
}

//------------------------------------------------------------------------------

bool mesh::has_color_layer_changed(const data::mesh::csptr& _mesh) const
{
    return _mesh->has<attribute::point_colors>() != data::mesh::has<attribute::point_colors>(m_layout)
           || _mesh->has<attribute::cell_colors>() != data::mesh::has<attribute::cell_colors>(m_layout);
}

//------------------------------------------------------------------------------

Ogre::Entity* mesh::create_entity(Ogre::SceneManager& _scene_mgr)
{
    return _scene_mgr.createEntity(m_ogre_mesh);
}

//------------------------------------------------------------------------------

void mesh::invalidate_r2vb()
{
    for(auto* val : m_r2vb_object | std::views::values)
    {
        val->set_dirty();
    }
}

//------------------------------------------------------------------------------

bool mesh::are_bounds_valid(const Ogre::MeshPtr& _ogre_mesh)
{
    const Ogre::AxisAlignedBox& bounds = _ogre_mesh->getBounds();
    const Ogre::Vector3& maximum       = bounds.getMaximum();
    const Ogre::Vector3& minimum       = bounds.getMinimum();

    return !maximum.isNaN() && !minimum.isNaN()
           && !std::isinf(maximum[0]) && !std::isinf(maximum[1]) && !std::isinf(maximum[2])
           && !std::isinf(minimum[0]) && !std::isinf(minimum[1]) && !std::isinf(minimum[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
