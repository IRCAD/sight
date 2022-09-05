/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "viz/scene3d/Mesh.hpp"

#include "viz/scene3d/R2VBRenderable.hpp"
#include <viz/scene3d/helper/Mesh.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <geometry/data/Mesh.hpp>

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

const unsigned int Mesh::s_maxTextureSize;

//-----------------------------------------------------------------------------

template<typename T>
void copyIndices(
    void* _cells,
    const data::Mesh& _mesh
)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* cells = static_cast<T*>(_cells);

    const auto type = _mesh.getCellType();

    if(type == data::Mesh::CellType::TRIANGLE)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::triangle>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
        }
    }
    else if(type == data::Mesh::CellType::QUAD)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::quad>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
            *cells++ = static_cast<T>(cell.pt[2]);
            *cells++ = static_cast<T>(cell.pt[3]);
        }
    }
    else if(type == data::Mesh::CellType::LINE)
    {
        for(const auto& cell : _mesh.crange<data::iterator::cell::line>())
        {
            *cells++ = static_cast<T>(cell.pt[0]);
            *cells++ = static_cast<T>(cell.pt[1]);
        }
    }
    else if(type == data::Mesh::CellType::TETRA)
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

Mesh::Mesh(const std::string& _name)
{
    m_binding[POSITION_NORMAL] = 0;
    m_binding[COLOUR]          = 0xFFFF;
    m_binding[TEXCOORD]        = 0xFFFF;

    auto& meshMgr = Ogre::MeshManager::getSingleton();

    // Create Mesh Data Structure
    const auto meshName = _name + "_Mesh";
    m_ogreMesh = meshMgr.createManual(meshName, viz::scene3d::RESOURCE_GROUP);

    // Create Mesh Data Structure for r2vb input
    // We could create it only when necessary, but for now it is simpler to create it every time
    const auto r2vbMeshName = _name + "_R2VBMesh";
    m_r2vbMesh = meshMgr.createManual(
        r2vbMeshName,
        viz::scene3d::RESOURCE_GROUP
    );

    // TODO: remove ?
    m_perPrimitiveColorTexture.reset();
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{
    // Destroy Ogre Mesh
    auto& meshMgr = Ogre::MeshManager::getSingleton();
    meshMgr.remove(m_ogreMesh->getHandle());
    meshMgr.remove(m_r2vbMesh->getHandle());
    m_ogreMesh.reset();
    m_r2vbMesh.reset();
}

//-----------------------------------------------------------------------------

void Mesh::bindLayer(
    const data::Mesh::csptr& _mesh,
    BufferBinding _binding,
    Ogre::VertexElementSemantic _semantic,
    Ogre::VertexElementType _type
)
{
    Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    Ogre::VertexDeclaration* vtxDecl = m_ogreMesh->sharedVertexData->vertexDeclaration;

    // Create the buffer semantic if it does not exist.
    if(vtxDecl->findElementBySemantic(_semantic) == nullptr)
    {
        m_binding[_binding] = static_cast<std::uint16_t>(bind->getBindings().size());

        vtxDecl->addElement(m_binding[_binding], 0, _type, _semantic);
    }

    // Get requested buffer size and previous buffer size.
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer;

    const std::size_t uiNumVertices = _mesh->numPoints();
    std::size_t uiPrevNumVertices   = 0;
    if(bind->isBufferBound(m_binding[_binding]))
    {
        vertex_buffer     = bind->getBuffer(m_binding[_binding]);
        uiPrevNumVertices = vertex_buffer->getNumVertices();
    }

    // Allocate the buffer if it necessary.
    if(!bind->isBufferBound(m_binding[_binding]) || uiPrevNumVertices < uiNumVertices)
    {
        FW_PROFILE_AVG("REALLOC LAYER", 5);

        // Allocate color buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
        Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                            ? Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                            : Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        const std::size_t offset = Ogre::VertexElement::getTypeSize(_type);

        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind->setBinding(m_binding[_binding], vertex_buffer);
    }
}

//------------------------------------------------------------------------------

void Mesh::setVisible(bool _visible)
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

void Mesh::updateMesh(const data::Mesh::csptr& _mesh, bool _pointsOnly)
{
    const auto dumpLock = _mesh->dump_lock();

    /// The values in this table refer to vertices in the above table
    const std::size_t numVertices = _mesh->numPoints();
    SIGHT_DEBUG("Vertices #" << numVertices);

    // Check if the mesh has normals - we assume we should have as many normals as points
    // If this is not the case, normals will be ignored or regenerated if needed and if the number of vertices changed
    m_hasNormal = _mesh->has<data::Mesh::Attributes::POINT_NORMALS>();

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(m_ogreMesh->sharedVertexData == nullptr)
    {
        m_ogreMesh->sharedVertexData = new Ogre::VertexData();
    }

    Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    std::size_t prevNumVertices     = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        prevNumVertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(!m_hasNormal && !_pointsOnly)
    {
        // Verify if mesh contains Tetra, Edge or Point
        // If not, generate normals

        const auto cellType       = _mesh->getCellType();
        const bool computeNormals = (cellType != data::Mesh::CellType::LINE
                                     && cellType != data::Mesh::CellType::TETRA
                                     && cellType != data::Mesh::CellType::POINT);

        if(computeNormals)
        {
            // /!\ DEPRECATED /!\: normals shouldn't be computed by an adaptor.
            // We need to remove the const of the _mesh to compute normals.
            geometry::data::Mesh::generatePointNormals(std::const_pointer_cast<data::Mesh>(_mesh));
            m_hasNormal = true;
        }
    }

    if(prevNumVertices < numVertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = numVertices;

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                            ? Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                            : Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::Mesh
        Ogre::VertexDeclaration* declMain = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        declMain->removeAllElements();

        // 1st buffer
        declMain->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        if(m_hasNormal)
        {
            declMain->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
            offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
        }

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, numVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = numVertices;
    }

    //------------------------------------------
    // Create indices arrays
    //------------------------------------------

    // 1 - Create a submesh for the primitive type
    const bool indices32Bits     = numVertices >= (1 << 16);
    const bool indicesPrev32Bits = prevNumVertices >= (1 << 16);
    const bool hasPrimitiveColor = _mesh->has<data::Mesh::Attributes::CELL_COLORS>();

    void* indexBuffer = nullptr;
    {
        FW_PROFILE_AVG("REALLOC INDEX", 5);

        const data::Mesh::CellType cellType     = _mesh->getCellType();
        const data::Mesh::CellType prevCellType = m_cellType;

        const bool destroyMesh = _mesh->numCells() == 0 || prevCellType != cellType
                                 || m_hasPrimitiveColor != hasPrimitiveColor;

        // Destroy the submesh if it has been created before - a submesh with 0 index would be invalid
        if(destroyMesh && (m_subMesh != nullptr))
        {
            std::string name = std::to_string(static_cast<int>(prevCellType));
            if((prevCellType == data::Mesh::CellType::TRIANGLE && hasPrimitiveColor)
               || (prevCellType == data::Mesh::CellType::TETRA || prevCellType == data::Mesh::CellType::QUAD))
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
                const std::string name = std::to_string(static_cast<int>(cellType));
                if(cellType == data::Mesh::CellType::TRIANGLE)
                {
                    if(hasPrimitiveColor)
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
                else if(cellType == data::Mesh::CellType::LINE)
                {
                    m_subMesh                = m_ogreMesh->createSubMesh(name);
                    m_subMesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
                }
                else if(cellType == data::Mesh::CellType::POINT)
                {
                    m_subMesh                = m_ogreMesh->createSubMesh(name);
                    m_subMesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
                }
                else if(cellType == data::Mesh::CellType::QUAD || cellType == data::Mesh::CellType::TETRA)
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

            if(cellType != data::Mesh::CellType::POINT)
            {
                Ogre::HardwareIndexBufferSharedPtr ibuf = m_subMesh->indexData->indexBuffer;
                const unsigned int numIndices           = _mesh->numCells() * _mesh->getCellSize();

                // Allocate index buffer of the requested number of vertices (ibufCount) if necessary
                // We don't reallocate if we have more space than requested
                bool createIndexBuffer = !ibuf;
                if(ibuf)
                {
                    // reallocate if new mesh has more indexes or IndexType change
                    createIndexBuffer = (ibuf->getNumIndexes() < numIndices
                                         || (indicesPrev32Bits != indices32Bits));
                }

                if(createIndexBuffer)
                {
                    Ogre::HardwareBuffer::Usage usage = m_isDynamic
                                                        ? Ogre::HardwareBuffer::
                                                        HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                                        :
                                                        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

                    ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                        indices32Bits ? Ogre::HardwareIndexBuffer::IT_32BIT : Ogre::HardwareIndexBuffer::IT_16BIT,
                        numIndices,
                        usage
                    );

                    m_subMesh->indexData->indexBuffer = ibuf;
                }

                m_subMesh->indexData->indexCount = numIndices;
                SIGHT_DEBUG("Index #" << m_subMesh->indexData->indexCount);

                // Lock index data, we are going to write into it in the next loop
                indexBuffer = ibuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);

                if(indices32Bits)
                {
                    copyIndices<std::uint32_t>(indexBuffer, *_mesh);
                }
                else
                {
                    copyIndices<std::uint16_t>(indexBuffer, *_mesh);
                }
            }
        }

        if(m_subMesh != nullptr && m_subMesh->indexData->indexBuffer)
        {
            m_subMesh->indexData->indexBuffer->unlock();
        }

        m_cellType = cellType;
    }
}

//------------------------------------------------------------------------------

void Mesh::updateMesh(const data::PointList::csptr& _pointList)
{
    auto points = _pointList->getPoints();

    /// The values in this table refer to vertices in the above table
    std::size_t uiNumVertices = points.size();
    SIGHT_DEBUG("Vertices #" << uiNumVertices);

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

    Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    std::size_t uiPrevNumVertices   = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        uiPrevNumVertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(uiPrevNumVertices < uiNumVertices)
    {
        FW_PROFILE("REALLOC MESH");

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
        Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                            ? Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                            : Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        std::size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::Mesh
        Ogre::VertexDeclaration* declMain = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        declMain->removeAllElements();

        // 1st buffer
        declMain->addElement(m_binding[POSITION_NORMAL], offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        Ogre::HardwareBufferManager& mgr = Ogre::HardwareBufferManager::getSingleton();
        vertex_buffer = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vertex_buffer);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;
    }

    if(m_subMesh == nullptr)
    {
        const auto pointType = static_cast<std::size_t>(data::Mesh::CellType::POINT);
        m_cellType               = data::Mesh::CellType::POINT;
        m_subMesh                = m_ogreMesh->createSubMesh(std::to_string(pointType));
        m_subMesh->operationType = Ogre::RenderOperation::OT_POINT_LIST;
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::vector<R2VBRenderable*> > Mesh::updateR2VB(
    const data::Mesh::csptr& _mesh,
    Ogre::SceneManager& _sceneMgr,
    const std::string& _materialName
)
{
    //------------------------------------------
    // Render to vertex-buffer
    // Three cases to handle:
    // - Quads primitives generation - 2 triangles from 4 points
    // - Tetrahedrons primitives generation - 4 triangles from 4 points
    // - Per-primitive color generation - either triangles, quads or tetrahedrons
    //------------------------------------------
    std::vector<R2VBRenderable*> r2vbRenderables;
    bool add = true;

    const bool hasPrimitiveColor = _mesh->has<data::Mesh::Attributes::CELL_COLORS>();
    const bool bQuad             = m_cellType == data::Mesh::CellType::QUAD;
    const bool bTetra            = m_cellType == data::Mesh::CellType::TETRA;
    if(bQuad || bTetra || hasPrimitiveColor)
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

        // Add mesh to Ogre Root Scene Node if it doesn't exist yet
        if(m_r2vbEntity == nullptr)
        {
            m_r2vbEntity = _sceneMgr.createEntity(m_r2vbMesh);
            // Never attach it to the scene manager otherwise it will be computed all the time
            _sceneMgr.getRootSceneNode()->detachObject(m_r2vbEntity);
        }

        const std::size_t numSubEntities = m_r2vbEntity->getNumSubEntities();
        for(std::size_t i = 0 ; i < numSubEntities ; ++i)
        {
            auto* const subEntity = m_r2vbEntity->getSubEntity(i);
            auto* const subMesh   = subEntity->getSubMesh();

            const data::Mesh::CellType cellType = bQuad ? data::Mesh::CellType::QUAD
                                                        : bTetra ? data::Mesh::CellType::TETRA
                                                                 : data::Mesh::CellType::TRIANGLE;

            if(m_r2vbObject.find(cellType) == m_r2vbObject.end())
            {
                const std::string name           = std::to_string(static_cast<int>(cellType));
                const std::string r2vbObjectName = m_ogreMesh->getName() + "_r2vbObject_" + name;
                m_r2vbObject[cellType] = viz::scene3d::R2VBRenderable::New(
                    r2vbObjectName,
                    subEntity,
                    &_sceneMgr,
                    cellType,
                    _materialName
                );
            }

            m_r2vbObject[cellType]->setOutputSettings(
                static_cast<unsigned int>(subMesh->indexData->indexCount),
                m_hasPrimitiveColor || m_hasVertexColor,
                m_hasUV
            );

            r2vbRenderables.push_back(m_r2vbObject[cellType]);
        }

        add = true;
    }
    else
    {
        // Clear if necessary
        for(auto r2vbObject : m_r2vbObject)
        {
            r2vbRenderables.push_back(r2vbObject.second);
        }

        m_r2vbObject.clear();

        if(m_r2vbEntity != nullptr)
        {
            _sceneMgr.destroyEntity(m_r2vbEntity);
            m_r2vbEntity = nullptr;
        }

        add = false;
    }

    return std::make_pair(add, r2vbRenderables);
}

//-----------------------------------------------------------------------------

void Mesh::updateVertices(const data::Mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with data::Mesh
    const auto dumpLock = _mesh->dump_lock();

    std::size_t uiStrideFloat = 3;
    if(m_hasNormal)
    {
        uiStrideFloat += 3;
    }

    using position_t = data::Mesh::position_t;
    using normal_t   = data::Mesh::normal_t;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    position_t xMin = std::numeric_limits<position_t>::max();
    position_t yMin = std::numeric_limits<position_t>::max();
    position_t zMin = std::numeric_limits<position_t>::max();
    position_t xMax = std::numeric_limits<position_t>::lowest();
    position_t yMax = std::numeric_limits<position_t>::lowest();
    position_t zMax = std::numeric_limits<position_t>::lowest();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(const auto& p : _mesh->crange<data::iterator::point::xyz>())
        {
            const auto& pt0 = p.x;
            xMin = std::min(xMin, pt0);
            xMax = std::max(xMax, pt0);

            const auto& pt1 = p.y;
            yMin = std::min(yMin, pt1);
            yMax = std::max(yMax, pt1);

            const auto& pt2 = p.z;
            zMin = std::min(zMin, pt2);
            zMax = std::max(zMax, pt2);
        }
    }
    {
        FW_PROFILE_AVG("UPDATE POS AND NORMALS", 5);
        auto* __restrict pPos = static_cast<position_t*>(pVertex);
        for(const auto& p : _mesh->crange<data::iterator::point::xyz>())
        {
            memcpy(pPos, &p.x, 3 * sizeof(position_t));
            pPos += uiStrideFloat;
        }

        normal_t* __restrict pNormal = nullptr;

        if(m_hasNormal)
        {
            pNormal = static_cast<normal_t*>(pVertex) + 3;

            for(const auto& n : _mesh->crange<data::iterator::point::nxyz>())
            {
                memcpy(pNormal, &n.nx, 3 * sizeof(normal_t));
                pNormal += uiStrideFloat;
            }
        }
    }

    // Unlock vertex data
    vertex_buffer->unlock();

    if(xMin < std::numeric_limits<position_t>::max()
       && yMin < std::numeric_limits<position_t>::max()
       && zMin < std::numeric_limits<position_t>::max()
       && xMax > std::numeric_limits<position_t>::lowest()
       && yMax > std::numeric_limits<position_t>::lowest()
       && zMax > std::numeric_limits<position_t>::lowest())
    {
        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox(xMin, yMin, zMin, xMax, yMax, zMax));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::Mesh::areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            m_ogreMesh->_setBoundingSphereRadius(
                Ogre::Math::Sqrt(
                    Ogre::Math::Sqr(xMax - xMin)
                    + Ogre::Math::Sqr(yMax - yMin)
                    + Ogre::Math::Sqr(zMax - zMin)
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

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateVertices(const data::PointList::csptr& _pointList)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    Ogre::VertexBufferBinding* bind                   = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with data::Mesh
    const std::size_t uiStrideFloat = 3;

    using PointType = data::Point::PointCoordType;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    PointType xMin = std::numeric_limits<PointType>::max();
    PointType yMin = std::numeric_limits<PointType>::max();
    PointType zMin = std::numeric_limits<PointType>::max();
    PointType xMax = std::numeric_limits<PointType>::lowest();
    PointType yMax = std::numeric_limits<PointType>::lowest();
    PointType zMax = std::numeric_limits<PointType>::lowest();

    const auto& points          = _pointList->getPoints();
    const std::size_t numPoints = points.size();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            const auto point = points[i]->getCoord();
            const auto& pt0  = point[0];
            xMin = std::min(xMin, pt0);
            xMax = std::max(xMax, pt0);

            const auto& pt1 = point[1];
            yMin = std::min(yMin, pt1);
            yMax = std::max(yMax, pt1);

            const auto& pt2 = point[2];
            zMin = std::min(zMin, pt2);
            zMax = std::max(zMax, pt2);
        }
    }
    {
        auto* __restrict pPos = static_cast<float*>(pVertex);
        FW_PROFILE_AVG("UPDATE POS", 5);
        for(std::size_t i = 0 ; i < numPoints ; ++i)
        {
            const auto point = points[i]->getCoord();
            pPos[0] = static_cast<float>(point[0]);
            pPos[1] = static_cast<float>(point[1]);
            pPos[2] = static_cast<float>(point[2]);

            pPos += uiStrideFloat;
        }
    }
    // Unlock vertex data
    vertex_buffer->unlock();

    if(xMin < std::numeric_limits<PointType>::max()
       && yMin < std::numeric_limits<PointType>::max()
       && zMin < std::numeric_limits<PointType>::max()
       && xMax > std::numeric_limits<PointType>::lowest()
       && yMax > std::numeric_limits<PointType>::lowest()
       && zMax > std::numeric_limits<PointType>::lowest())
    {
        const auto xMinF = static_cast<float>(xMin);
        const auto yMinF = static_cast<float>(yMin);
        const auto zMinF = static_cast<float>(zMin);
        const auto xMaxF = static_cast<float>(xMax);
        const auto yMaxF = static_cast<float>(yMax);
        const auto zMaxF = static_cast<float>(zMax);

        m_ogreMesh->_setBounds(Ogre::AxisAlignedBox(xMinF, yMinF, zMinF, xMaxF, yMaxF, zMaxF));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = sight::viz::scene3d::Mesh::areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            const float xLenF = xMaxF - xMinF;
            const float yLenF = yMaxF - yMinF;
            const float zLenF = zMaxF - xMinF;

            m_ogreMesh->_setBoundingSphereRadius(
                Ogre::Math::Sqrt(
                    Ogre::Math::Sqr(xLenF)
                    + Ogre::Math::Sqr(yLenF)
                    + Ogre::Math::Sqr(zLenF)
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

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateColors(const data::Mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    const bool hasVertexColor    = _mesh->has<data::Mesh::Attributes::POINT_COLORS>();
    const bool hasPrimitiveColor = _mesh->has<data::Mesh::Attributes::CELL_COLORS>();

    // 1 - Initialization
    if(hasVertexColor)
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

    if(hasPrimitiveColor)
    {
        unsigned int numIndicesTotal = 0;

        if(m_subMesh != nullptr)
        {
            if(_mesh->getCellType() == data::Mesh::CellType::TRIANGLE)
            {
                numIndicesTotal += static_cast<unsigned int>(m_subMesh->indexData->indexCount / 3);
            }
            else
            {
                numIndicesTotal += static_cast<unsigned int>(m_subMesh->indexData->indexCount >> 2);
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

        const std::size_t width = std::min(s_maxTextureSize, numIndicesTotal);
        const auto height       = static_cast<std::size_t>(std::floor(numIndicesTotal / s_maxTextureSize) + 1);

        if(m_perPrimitiveColorTexture->getWidth() != width || m_perPrimitiveColorTexture->getHeight() != height)
        {
            FW_PROFILE_AVG("REALLOC COLORS_CELL", 5);

            // It would be better to use PF_BYTE_RGB when we have 3 components but for some reason it doesn't work
            // Probably something related to alignment or a bug in Ogre
            viz::scene3d::Utils::allocateTexture(
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
            m_perPrimitiveColorTexture->freeInternalResources();
        }

        m_perPrimitiveColorTexture.reset();
        m_perPrimitiveColorTextureName = "";
    }

    const auto lock = _mesh->dump_lock();
    // 2 - Copy of vertices
    if(hasVertexColor)
    {
        // Source points
        Ogre::HardwareVertexBufferSharedPtr vertex_buffer = bind->getBuffer(m_binding[COLOUR]);
        auto* pColor                                      =
            static_cast<Ogre::RGBA*>(vertex_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::point::rgba>())->r;

        // Copy points
        const std::size_t nbComponents = 4;
        viz::scene3d::helper::Mesh::copyColors(pColor, colors, _mesh->numPoints(), nbComponents);

        vertex_buffer->unlock();
    }

    if(hasPrimitiveColor)
    {
        // Source cells
        Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_perPrimitiveColorTexture->getBuffer();
        pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        auto* pColorDest               = reinterpret_cast<Ogre::RGBA*>(pixelBox.data);

        // Destination
        const std::uint8_t* colors = &(_mesh->cbegin<data::iterator::cell::rgba>())->r;

        // Copy cells
        const std::size_t nbComponents = 4;
        viz::scene3d::helper::Mesh::copyColors(pColorDest, colors, _mesh->numCells(), nbComponents);

        pixelBuffer->unlock();
    }

    if(hasVertexColor != m_hasVertexColor || hasPrimitiveColor != m_hasPrimitiveColor)
    {
        m_hasVertexColor    = hasVertexColor;
        m_hasPrimitiveColor = hasPrimitiveColor;
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateTexCoords(const data::Mesh::csptr& _mesh)
{
    m_hasUV = _mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>();

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if(m_hasUV)
    {
        bindLayer(_mesh, TEXCOORD, Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        Ogre::VertexBufferBinding* bind               = m_ogreMesh->sharedVertexData->vertexBufferBinding;
        Ogre::HardwareVertexBufferSharedPtr uv_buffer = bind->getBuffer(m_binding[TEXCOORD]);
        void* pBuf                                    = uv_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        auto* pUV                                     = static_cast<float*>(pBuf);

        // Copy UV coordinates for each mesh point
        for(const auto& uv : _mesh->crange<data::iterator::point::uv>())
        {
            pUV[0] = uv.u;
            pUV[1] = uv.v;
            pUV   += 2;
        }

        uv_buffer->unlock();
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::clearMesh(Ogre::SceneManager& _sceneMgr)
{
    // Destroy all the submeshes, but keep the two meshes alive

    if(m_subMesh != nullptr)
    {
        const data::Mesh::CellType cellType = m_cellType;
        const std::string name              = std::to_string(static_cast<int>(cellType));
        if((cellType == data::Mesh::CellType::TRIANGLE && m_hasPrimitiveColor)
           || (cellType == data::Mesh::CellType::TETRA || cellType == data::Mesh::CellType::QUAD))
        {
            m_r2vbMesh->destroySubMesh(name);
        }
        else
        {
            m_ogreMesh->destroySubMesh(name);
        }

        m_subMesh = nullptr;
    }

    for(auto r2vbObject : m_r2vbObject)
    {
        _sceneMgr.destroyMovableObject(r2vbObject.second);
    }

    m_r2vbObject.clear();

    if(m_r2vbEntity != nullptr)
    {
        _sceneMgr.destroyEntity(m_r2vbEntity);
        m_r2vbEntity = nullptr;
    }
}

//------------------------------------------------------------------------------

void Mesh::updateMaterial(Material* _material, bool _isR2VB) const
{
    auto bbox = m_ogreMesh->getBounds();
    _material->setMeshSize(bbox.getSize().length());

    _material->setHasMeshNormal(m_hasNormal);
    _material->setHasUV(m_hasUV);

    // The r2vb pipeline outputs per-vertex color if we have per-primitive color
    // Thus for the rendering pipeline it is only viewed as per-vertex color
    if(_isR2VB)
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

bool Mesh::hasColorLayerChanged(const data::Mesh::csptr& _mesh) const
{
    const bool hasVertexColor    = _mesh->has<data::Mesh::Attributes::POINT_COLORS>();
    const bool hasPrimitiveColor = _mesh->has<data::Mesh::Attributes::CELL_COLORS>();

    return hasVertexColor != m_hasVertexColor || hasPrimitiveColor != m_hasPrimitiveColor;
}

//------------------------------------------------------------------------------

Ogre::Entity* Mesh::createEntity(Ogre::SceneManager& _sceneMgr)
{
    return _sceneMgr.createEntity(m_ogreMesh);
}

//------------------------------------------------------------------------------

void Mesh::invalidateR2VB()
{
    for(auto r2vbObject : m_r2vbObject)
    {
        r2vbObject.second->setDirty();
    }
}

//------------------------------------------------------------------------------

bool Mesh::areBoundsValid(const Ogre::MeshPtr& _ogreMesh)
{
    const Ogre::AxisAlignedBox& bounds = _ogreMesh->getBounds();
    const Ogre::Vector3& maximum       = bounds.getMaximum();
    const Ogre::Vector3& minimum       = bounds.getMinimum();

    return !maximum.isNaN() && !minimum.isNaN()
           && !std::isinf(maximum[0]) && !std::isinf(maximum[1]) && !std::isinf(maximum[2])
           && !std::isinf(minimum[0]) && !std::isinf(minimum[1]) && !std::isinf(minimum[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
