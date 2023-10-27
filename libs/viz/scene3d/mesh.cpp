/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

namespace sight::viz::scene3d
{

const unsigned int mesh::s_maxTextureSize;

//-----------------------------------------------------------------------------

template<typename T>
void copy_indices(
    void* _cells,
    const data::mesh& _mesh
)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* cells = static_cast<T*>(_cells);

    const auto type = _mesh.get_cell_type();

    if(type == data::mesh::cell_type_t::TRIANGLE)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::triangle>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
        }
    }
    else if(type == data::mesh::cell_type_t::QUAD)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::quad>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
            *cells++ = static_cast<T>(cell.pt[3]);
        }
    }
    else if(type == data::mesh::cell_type_t::LINE)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::line>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
        }
    }
    else if(type == data::mesh::cell_type_t::TETRA)
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
    m_binding[POSITION_NORMAL] = 0;
    m_binding[COLOUR]          = 0xFFFF;
    m_binding[TEXCOORD]        = 0xFFFF;

    auto& mesh_mgr = Ogre::MeshManager::getSingleton();

    // Create mesh Data Structure
    const auto mesh_name = _name + "_Mesh";
    m_ogreMesh = mesh_mgr.createManual(mesh_name, viz::scene3d::RESOURCE_GROUP);

    // Create mesh Data Structure for r2vb input
    // We could create it only when necessary, but for now it is simpler to create it every time
    const auto r2vb_mesh_name = _name + "_R2VBMesh";
    m_r2vbMesh = mesh_mgr.createManual(
        r2vb_mesh_name,
        viz::scene3d::RESOURCE_GROUP
    );

    // TODO: remove ?
    m_perPrimitiveColorTexture.reset();
}

//------------------------------------------------------------------------------

mesh::~mesh()
{
    // Destroy Ogre mesh
    auto& mesh_mgr = Ogre::MeshManager::getSingleton();
    mesh_mgr.remove(m_ogreMesh->getHandle());
    mesh_mgr.remove(m_r2vbMesh->getHandle());
    m_ogreMesh.reset();
    m_r2vbMesh.reset();
}

//-----------------------------------------------------------------------------

void mesh::bindLayer(
    const data::mesh::csptr& _mesh,
    BufferBinding _binding,
    Ogre::VertexElementSemantic _semantic,
    Ogre::VertexElementType _type
)
{
    Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    Ogre::VertexDeclaration* vtx_decl = m_ogreMesh->sharedVertexData->vertexDeclaration;

    // Create the buffer semantic if it does not exist.
    if(vtx_decl->findElementBySemantic(_semantic) == nullptr)
    {
        m_binding[_binding] = static_cast<std::uint16_t>(bind->getBindings().size());

        vtx_decl->addElement(m_binding[_binding], 0, _type, _semantic);
    }

    // Get requested buffer size and previous buffer size.
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer;

    const std::size_t ui_num_vertices = _mesh->numPoints();
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
            (m_isDynamic || m_isDynamicVertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        const std::size_t offset = Ogre::VertexElement::getTypeSize(_type);

        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, ui_num_vertices, usage, false);
        bind->setBinding(m_binding[_binding], vertex_buffer);
    }
}

//------------------------------------------------------------------------------

void mesh::setVisible(bool _visible)
{
    if(m_r2vbEntity != nullptr)
    {
        m_r2vbEntity->setVisible(_visible);
    }

    for(auto& it : m_r2vbObject)
    {
        it.second->setVisible(_visible);
    }
}

//------------------------------------------------------------------------------

void mesh::updateMesh(const data::mesh::csptr& _mesh, bool _points_only)
{
    const auto dump_lock = _mesh->dump_lock();

    /// The values in this table refer to vertices in the above table
    const std::size_t num_vertices = _mesh->numPoints();
    SIGHT_DEBUG("Vertices #" << num_vertices);

    // Check if the mesh has normals - we assume we should have as many normals as points
    // If this is not the case, normals will be ignored or regenerated if needed and if the number of vertices changed
    m_hasNormal = _mesh->has<data::mesh::Attributes::POINT_NORMALS>();

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(m_ogreMesh->sharedVertexData == nullptr)
    {
        m_ogreMesh->sharedVertexData = new Ogre::VertexData();
    }

    Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    std::size_t prev_num_vertices   = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        prev_num_vertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(!m_hasNormal && !_points_only)
    {
        // Verify if mesh contains Tetra, Edge or Point
        // If not, generate normals

        const auto cell_type       = _mesh->get_cell_type();
        const bool compute_normals = (cell_type != data::mesh::cell_type_t::LINE
                                      && cell_type != data::mesh::cell_type_t::TETRA
                                      && cell_type != data::mesh::cell_type_t::POINT);

        if(compute_normals)
        {
            // /!\ DEPRECATED /!\: normals shouldn't be computed by an adaptor.
            // We need to remove the const of the _mesh to compute normals.
            geometry::data::mesh::generatePointNormals(std::const_pointer_cast<data::mesh>(_mesh));
            m_hasNormal = true;
        }
    }

    if(prev_num_vertices < num_vertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = Ogre::uint32(num_vertices);

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        auto usage =
            (m_isDynamic || m_isDynamicVertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::mesh
        Ogre::VertexDeclaration* decl_main = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        decl_main->removeAllElements();

        // 1st buffer
        decl_main->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        if(m_hasNormal)
        {
            decl_main->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
            offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
        }

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, num_vertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = Ogre::uint32(num_vertices);
    }

    //------------------------------------------
    // Create indices arrays
    //------------------------------------------

    // 1 - Create a submesh for the primitive type
    const bool indices32_bits      = num_vertices >= (1 << 16);
    const bool indices_prev32_bits = prev_num_vertices >= (1 << 16);
    const bool has_primitive_color = _mesh->has<data::mesh::Attributes::CELL_COLORS>();
    {
        FW_PROFILE_AVG("REALLOC INDEX", 5);

        const data::mesh::cell_type_t cell_type      = _mesh->get_cell_type();
        const data::mesh::cell_type_t prev_cell_type = m_cellType;

        const bool destroy_mesh = _mesh->numCells() == 0 || prev_cell_type != cell_type
                                  || m_hasPrimitiveColor != has_primitive_color;

        // Destroy the submesh if it has been created before - a submesh with 0 index would be invalid
        if(destroy_mesh && (m_subMesh != nullptr))
        {
            std::string name = std::to_string(static_cast<int>(prev_cell_type));
            if((prev_cell_type == data::mesh::cell_type_t::TRIANGLE && has_primitive_color)
               || (prev_cell_type == data::mesh::cell_type_t::TETRA || prev_cell_type == data::mesh::cell_type_t::QUAD))
            {
                m_r2vbMesh->destroySubMesh(name);
            }
            else
            {
                m_ogreMesh->destroySubMesh(name);
            }

            m_subMesh = nullptr;
        }

        if(_mesh->numCells() > 0)
        {
            if(m_subMesh == nullptr)
            {
                // Create one submesh
                const std::string name = std::to_string(static_cast<int>(cell_type));
                if(cell_type == data::mesh::cell_type_t::TRIANGLE)
                {
                    if(has_primitive_color)
                    {
                        // Use r2vb pipeline for per-primitive color
                        m_subMesh                = m_r2vbMesh->createSubMesh(name);
                        m_subMesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST_ADJ;
                    }
                    else
                    {
                        m_subMesh                = m_ogreMesh->createSubMesh(name);
                        m_subMesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
                    }
                }
                else if(cell_type == data::mesh::cell_type_t::LINE)
                {
                    m_subMesh                = m_ogreMesh->createSubMesh(name);
                    m_subMesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
                }
                else if(cell_type == data::mesh::cell_type_t::POINT)
                {
                    m_subMesh                = m_ogreMesh->createSubMesh(name);
                    m_subMesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
                }
                else if(cell_type == data::mesh::cell_type_t::QUAD || cell_type == data::mesh::cell_type_t::TETRA)
                {
                    // Use r2vb pipeline to generate quads or tetrahedrons
                    m_subMesh                = m_r2vbMesh->createSubMesh(name);
                    m_subMesh->operationType = Ogre::RenderOperation::OT_LINE_LIST_ADJ;
                }
                else
                {
                    SIGHT_ASSERT("invalid cell type", false);
                }

                m_subMesh->useSharedVertices     = true;
                m_subMesh->indexData->indexStart = 0;
            }

            if(cell_type != data::mesh::cell_type_t::POINT)
            {
                Ogre::HardwareIndexBufferSharedPtr ibuf = m_subMesh->indexData->indexBuffer;
                const unsigned int num_indices          = _mesh->numCells() * _mesh->getCellSize();

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
                        m_isDynamic
                        ? static_cast<Ogre::HardwareBuffer::Usage>(
                            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                        )
                        : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

                    ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                        indices32_bits ? Ogre::HardwareIndexBuffer::IT_32BIT : Ogre::HardwareIndexBuffer::IT_16BIT,
                        num_indices,
                        usage
                    );

                    m_subMesh->indexData->indexBuffer = ibuf;
                }

                m_subMesh->indexData->indexCount = num_indices;
                SIGHT_DEBUG("Index #" << m_subMesh->indexData->indexCount);

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

        if(m_subMesh != nullptr && m_subMesh->indexData->indexBuffer)
        {
            m_subMesh->indexData->indexBuffer->unlock();
        }

        m_cellType = cell_type;
    }
}

//------------------------------------------------------------------------------

void mesh::updateMesh(const data::point_list::csptr& _point_list)
{
    auto points = _point_list->getPoints();

    /// The values in this table refer to vertices in the above table
    std::size_t ui_num_vertices = points.size();
    SIGHT_DEBUG("Vertices #" << ui_num_vertices);

    // Check if mesh attributes
    m_hasNormal = false;

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(m_ogreMesh->sharedVertexData == nullptr)
    {
        m_ogreMesh->sharedVertexData = new Ogre::VertexData();
    }

    Ogre::VertexBufferBinding& bind  = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    std::size_t ui_prev_num_vertices = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        ui_prev_num_vertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(ui_prev_num_vertices < ui_num_vertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = Ogre::uint32(ui_num_vertices);

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        auto usage =
            (m_isDynamic || m_isDynamicVertices)
            ? static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE)
            : static_cast<Ogre::HardwareBuffer::Usage>(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::mesh
        Ogre::VertexDeclaration* decl_main = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        decl_main->removeAllElements();

        // 1st buffer
        decl_main->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, ui_num_vertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = Ogre::uint32(ui_num_vertices);
    }

    if(m_subMesh == nullptr)
    {
        const auto point_type = static_cast<std::size_t>(data::mesh::cell_type_t::POINT);
        m_cellType               = data::mesh::cell_type_t::POINT;
        m_subMesh                = m_ogreMesh->createSubMesh(std::to_string(point_type));
        m_subMesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::vector<r2vb_renderable*> > mesh::updateR2VB(
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

    const bool has_primitive_color = _mesh->has<data::mesh::Attributes::CELL_COLORS>();
    const bool b_quad              = m_cellType == data::mesh::cell_type_t::QUAD;
    const bool b_tetra             = m_cellType == data::mesh::cell_type_t::TETRA;
    if(b_quad || b_tetra || has_primitive_color)
    {
        if(m_r2vbMesh->sharedVertexData == nullptr)
        {
            m_r2vbMesh->sharedVertexData = new Ogre::VertexData();
        }

        *m_r2vbMesh->sharedVertexData->vertexBufferBinding = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
        *m_r2vbMesh->sharedVertexData->vertexDeclaration   = *m_ogreMesh->sharedVertexData->vertexDeclaration;
        m_r2vbMesh->sharedVertexData->vertexCount          = m_ogreMesh->sharedVertexData->vertexCount;

        m_r2vbMesh->_setBounds(m_ogreMesh->getBounds());
        m_r2vbMesh->_setBoundingSphereRadius(m_ogreMesh->getBoundingSphereRadius());

        // Add mesh to Ogre Root scene Node if it doesn't exist yet
        if(m_r2vbEntity == nullptr)
        {
            m_r2vbEntity = _scene_mgr.createEntity(m_r2vbMesh);
        }

        const std::size_t num_sub_entities = m_r2vbEntity->getNumSubEntities();
        for(std::size_t i = 0 ; i < num_sub_entities ; ++i)
        {
            auto* const sub_entity = m_r2vbEntity->getSubEntity(i);
            auto* const sub_mesh   = sub_entity->getSubMesh();

            const data::mesh::cell_type_t cell_type = b_quad ? data::mesh::cell_type_t::QUAD
                                                             : b_tetra ? data::mesh::cell_type_t::TETRA
                                                                       : data::mesh::cell_type_t::TRIANGLE;

            if(m_r2vbObject.find(cell_type) == m_r2vbObject.end())
            {
                const std::string name             = std::to_string(static_cast<int>(cell_type));
                const std::string r2vb_object_name = m_ogreMesh->getName() + "_r2vbObject_" + name;
                m_r2vbObject[cell_type] = viz::scene3d::r2vb_renderable::make(
                    r2vb_object_name,
                    sub_entity,
                    &_scene_mgr,
                    cell_type,
                    _material_name
                );
            }

            m_r2vbObject[cell_type]->setOutputSettings(
                static_cast<unsigned int>(sub_mesh->indexData->indexCount),
                m_hasPrimitiveColor || m_hasVertexColor,
                m_hasUV
            );

            r2vb_renderables.push_back(m_r2vbObject[cell_type]);
        }

        add = true;
    }
    else
    {
        // Clear if necessary
        for(auto r2vb_object : m_r2vbObject)
        {
            r2vb_renderables.push_back(r2vb_object.second);
        }

        m_r2vbObject.clear();

        if(m_r2vbEntity != nullptr)
        {
            _scene_mgr.destroyEntity(m_r2vbEntity);
            m_r2vbEntity = nullptr;
        }

        add = false;
    }

    return std::make_pair(add, r2vb_renderables);
}

//-----------------------------------------------------------------------------

void mesh::updateVertices(const data::mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* p_vertex = vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre mesh with data::mesh
    const auto dump_lock = _mesh->dump_lock();

    std::size_t ui_stride_float = 3;
    if(m_hasNormal)
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

        if(m_hasNormal)
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
        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox(x_min, y_min, z_min, x_max, y_max, z_max));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::mesh::areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            m_ogreMesh->_setBoundingSphereRadius(
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
            m_ogreMesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void mesh::updateVertices(const data::point_list::csptr& _point_list)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[POSITION_NORMAL]);

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

    const auto& points           = _point_list->getPoints();
    const std::size_t num_points = points.size();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(std::size_t i = 0 ; i < num_points ; ++i)
        {
            const auto point = points[i]->getCoord();
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
            const auto point = points[i]->getCoord();
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

        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox(x_min_f, y_min_f, z_min_f, x_max_f, y_max_f, z_max_f));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::mesh::areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            const float x_len_f = x_max_f - x_min_f;
            const float y_len_f = y_max_f - y_min_f;
            const float z_len_f = z_max_f - x_min_f;

            m_ogreMesh->_setBoundingSphereRadius(
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
            m_ogreMesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void mesh::updateColors(const data::mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    const bool has_vertex_color    = _mesh->has<data::mesh::Attributes::POINT_COLORS>();
    const bool has_primitive_color = _mesh->has<data::mesh::Attributes::CELL_COLORS>();

    // 1 - Initialization
    if(has_vertex_color)
    {
        bindLayer(_mesh, COLOUR, Ogre::VES_DIFFUSE, Ogre::VET_COLOUR);
    }
    else
    {
        // Unbind vertex color if it was previously enabled
        if(bind->isBufferBound(m_binding[COLOUR]))
        {
            bind->unsetBinding(m_binding[COLOUR]);
            m_binding[COLOUR] = 0xFFFF;
        }
    }

    if(has_primitive_color)
    {
        unsigned int num_indices_total = 0;

        if(m_subMesh != nullptr)
        {
            if(_mesh->get_cell_type() == data::mesh::cell_type_t::TRIANGLE)
            {
                num_indices_total += static_cast<unsigned int>(m_subMesh->indexData->indexCount / 3);
            }
            else
            {
                num_indices_total += static_cast<unsigned int>(m_subMesh->indexData->indexCount >> 2);
            }
        }

        if(!m_perPrimitiveColorTexture)
        {
            static std::uint64_t i = 0;
            m_perPrimitiveColorTextureName = "PerCellColorTexture_" + std::to_string(i++);
            m_perPrimitiveColorTexture     = Ogre::TextureManager::getSingleton().create(
                m_perPrimitiveColorTextureName,
                viz::scene3d::RESOURCE_GROUP,
                true
            );
        }

        const std::size_t width = std::min(s_maxTextureSize, num_indices_total);
        const auto height       = static_cast<std::size_t>(std::floor(num_indices_total / s_maxTextureSize) + 1);

        if(m_perPrimitiveColorTexture->getWidth() != width || m_perPrimitiveColorTexture->getHeight() != height)
        {
            FW_PROFILE_AVG("REALLOC COLORS_CELL", 5);

            // It would be better to use PF_BYTE_RGB when we have 3 components but for some reason it doesn't work
            // Probably something related to alignment or a bug in Ogre
            viz::scene3d::utils::allocateTexture(
                m_perPrimitiveColorTexture.get(),
                width,
                height,
                1,
                Ogre::PF_BYTE_RGBA,
                Ogre::TEX_TYPE_2D,
                (m_isDynamic || m_isDynamicVertices)
            );
        }
    }
    else
    {
        if(m_perPrimitiveColorTexture)
        {
            m_perPrimitiveColorTexture->unload();
        }

        m_perPrimitiveColorTexture.reset();
        m_perPrimitiveColorTextureName = "";
    }

    const auto lock = _mesh->dump_lock();
    // 2 - Copy of vertices
    if(has_vertex_color)
    {
        // Source points
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[COLOUR]);
        auto* p_color                                     =
            static_cast<Ogre::RGBA*>(vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::point::rgba>())->r;

        // Copy points
        const std::size_t nb_components = 4;
        viz::scene3d::helper::mesh::copyColors(p_color, colors, _mesh->numPoints(), nb_components);

        vertex_buffer->unlock();
    }

    if(has_primitive_color)
    {
        // Source cells
        Ogre::HardwarePixelBufferSharedPtr pixel_buffer = m_perPrimitiveColorTexture->getBuffer();
        pixel_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixel_box = pixel_buffer->getCurrentLock();
        auto* p_color_dest              = reinterpret_cast<Ogre::RGBA*>(pixel_box.data);

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::cell::rgba>())->r;

        // Copy cells
        const std::size_t nb_components = 4;
        viz::scene3d::helper::mesh::copyColors(p_color_dest, colors, _mesh->numCells(), nb_components);

        pixel_buffer->unlock();
    }

    if(has_vertex_color != m_hasVertexColor || has_primitive_color != m_hasPrimitiveColor)
    {
        m_hasVertexColor    = has_vertex_color;
        m_hasPrimitiveColor = has_primitive_color;
    }

    /// Notify mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void mesh::updateTexCoords(const data::mesh::csptr& _mesh)
{
    m_hasUV = _mesh->has<data::mesh::Attributes::POINT_TEX_COORDS>();

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if(m_hasUV)
    {
        bindLayer(_mesh, TEXCOORD, Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        Ogre::VertexBufferBinding* bind               = m_ogreMesh->sharedVertexData->vertexBufferBinding;
        Ogre::HardwareVertexBufferSharedPtr uv_buffer = bind->getBuffer(m_binding[TEXCOORD]);
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
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void mesh::clearMesh(Ogre::SceneManager& _scene_mgr)
{
    // Destroy all the submeshes, but keep the two meshes alive

    if(m_subMesh != nullptr)
    {
        const data::mesh::cell_type_t cell_type = m_cellType;
        const std::string name                  = std::to_string(static_cast<int>(cell_type));
        if((cell_type == data::mesh::cell_type_t::TRIANGLE && m_hasPrimitiveColor)
           || (cell_type == data::mesh::cell_type_t::TETRA || cell_type == data::mesh::cell_type_t::QUAD))
        {
            m_r2vbMesh->destroySubMesh(name);
        }
        else
        {
            m_ogreMesh->destroySubMesh(name);
        }

        m_subMesh = nullptr;
    }

    for(auto r2vb_object : m_r2vbObject)
    {
        _scene_mgr.destroyMovableObject(r2vb_object.second);
    }

    m_r2vbObject.clear();

    if(m_r2vbEntity != nullptr)
    {
        _scene_mgr.destroyEntity(m_r2vbEntity);
        m_r2vbEntity = nullptr;
    }
}

//------------------------------------------------------------------------------

void mesh::updateMaterial(material* _material, bool _is_r2_vb) const
{
    auto bbox = m_ogreMesh->getBounds();
    _material->setMeshSize(bbox.getSize().length());

    _material->setHasMeshNormal(m_hasNormal);
    _material->setHasUV(m_hasUV);

    // The r2vb pipeline outputs per-vertex color if we have per-primitive color
    // Thus for the rendering pipeline it is only viewed as per-vertex color
    if(_is_r2_vb)
    {
        _material->setHasVertexColor(m_hasVertexColor);
        _material->setHasPrimitiveColor(m_hasPrimitiveColor, m_perPrimitiveColorTextureName);
    }
    else
    {
        _material->setHasVertexColor(m_hasVertexColor || m_hasPrimitiveColor);
    }
}

//------------------------------------------------------------------------------

bool mesh::hasColorLayerChanged(const data::mesh::csptr& _mesh) const
{
    const bool has_vertex_color    = _mesh->has<data::mesh::Attributes::POINT_COLORS>();
    const bool has_primitive_color = _mesh->has<data::mesh::Attributes::CELL_COLORS>();

    return has_vertex_color != m_hasVertexColor || has_primitive_color != m_hasPrimitiveColor;
}

//------------------------------------------------------------------------------

Ogre::Entity* mesh::createEntity(Ogre::SceneManager& _scene_mgr)
{
    return _scene_mgr.createEntity(m_ogreMesh);
}

//------------------------------------------------------------------------------

void mesh::invalidateR2VB()
{
    for(auto r2vb_object : m_r2vbObject)
    {
        r2vb_object.second->setDirty();
    }
}

//------------------------------------------------------------------------------

bool mesh::areBoundsValid(const Ogre::MeshPtr& _ogre_mesh)
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
