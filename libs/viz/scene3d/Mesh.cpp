/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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
    void* _pTriangles,
    void* _pQuads,
    void* _pEdges,
    void* _pTetras,
    const data::Mesh::csptr& _mesh
)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* pTriangles = static_cast<T*>(_pTriangles);
    T* pQuads     = static_cast<T*>(_pQuads);
    T* pEdges     = static_cast<T*>(_pEdges);
    T* pTetras    = static_cast<T*>(_pTetras);

    auto cellItr       = _mesh->begin<data::iterator::ConstCellIterator>();
    const auto cellEnd = _mesh->end<data::iterator::ConstCellIterator>();

    for( ; cellItr != cellEnd ; ++cellItr)
    {
        const auto type = *cellItr->type;
        if(type == data::Mesh::CellType::TRIANGLE)
        {
            *pTriangles++ = static_cast<T>(cellItr->pointIdx[0]);
            *pTriangles++ = static_cast<T>(cellItr->pointIdx[1]);
            *pTriangles++ = static_cast<T>(cellItr->pointIdx[2]);
        }
        else if(type == data::Mesh::CellType::QUAD)
        {
            *pQuads++ = static_cast<T>(cellItr->pointIdx[0]);
            *pQuads++ = static_cast<T>(cellItr->pointIdx[1]);
            *pQuads++ = static_cast<T>(cellItr->pointIdx[2]);
            *pQuads++ = static_cast<T>(cellItr->pointIdx[3]);
        }
        else if(type == data::Mesh::CellType::EDGE)
        {
            *pEdges++ = static_cast<T>(cellItr->pointIdx[0]);
            *pEdges++ = static_cast<T>(cellItr->pointIdx[1]);
        }
        else if(type == data::Mesh::CellType::TETRA)
        {
            *pTetras++ = static_cast<T>(cellItr->pointIdx[0]);
            *pTetras++ = static_cast<T>(cellItr->pointIdx[1]);
            *pTetras++ = static_cast<T>(cellItr->pointIdx[2]);
            *pTetras++ = static_cast<T>(cellItr->pointIdx[3]);
        }
    }
}

//------------------------------------------------------------------------------

Mesh::Mesh(const std::string& _name)
{
    m_subMeshes.fill(nullptr);

    m_binding[POSITION_NORMAL] = 0;
    m_binding[COLOUR]          = 0xFFFF;
    m_binding[TEXCOORD]        = 0xFFFF;

    auto& meshMgr = ::Ogre::MeshManager::getSingleton();

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
    auto& meshMgr = ::Ogre::MeshManager::getSingleton();
    meshMgr.remove(m_ogreMesh->getHandle());
    meshMgr.remove(m_r2vbMesh->getHandle());
    m_ogreMesh.reset();
    m_r2vbMesh.reset();
}

//-----------------------------------------------------------------------------

void Mesh::bindLayer(
    const data::Mesh::csptr& _mesh,
    BufferBinding _binding,
    ::Ogre::VertexElementSemantic _semantic,
    ::Ogre::VertexElementType _type
)
{
    ::Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    ::Ogre::VertexDeclaration* vtxDecl = m_ogreMesh->sharedVertexData->vertexDeclaration;

    // Create the buffer semantic if it does not exist.
    if(!vtxDecl->findElementBySemantic(_semantic))
    {
        m_binding[_binding] = static_cast<unsigned short>(bind->getBindings().size());

        vtxDecl->addElement(m_binding[_binding], 0, _type, _semantic);
    }

    // Get requested buffer size and previous buffer size.
    ::Ogre::HardwareVertexBufferSharedPtr cbuf;

    const size_t uiNumVertices = _mesh->getNumberOfPoints();
    size_t uiPrevNumVertices   = 0;
    if(bind->isBufferBound(m_binding[_binding]))
    {
        cbuf              = bind->getBuffer(m_binding[_binding]);
        uiPrevNumVertices = cbuf->getNumVertices();
    }

    // Allocate the buffer if it necessary.
    if(!bind->isBufferBound(m_binding[_binding]) || uiPrevNumVertices < uiNumVertices)
    {
        FW_PROFILE_AVG("REALLOC LAYER", 5);

        // Allocate color buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                              ? ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                              : ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        const size_t offset = ::Ogre::VertexElement::getTypeSize(_type);

        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();
        cbuf = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind->setBinding(m_binding[_binding], cbuf);
    }
}

//------------------------------------------------------------------------------

void Mesh::setVisible(bool _visible)
{
    if(m_r2vbEntity)
    {
        m_r2vbEntity->setVisible(_visible);
    }

    for(auto& it : m_r2vbObject)
    {
        it.second->setVisible(_visible);
    }
}

//------------------------------------------------------------------------------

void Mesh::updateMesh(const data::Mesh::sptr& _mesh, bool _pointsOnly)
{
    const auto dumpLock = _mesh->lock();

    /// The values in this table refer to vertices in the above table
    const size_t numVertices = _mesh->getNumberOfPoints();
    SIGHT_DEBUG("Vertices #" << numVertices);

    // Check if the mesh has normals - we assume we should have as many normals as points
    // If this is not the case, normals will be ignored or regenerated if needed and if the number of vertices changed
    m_hasNormal = _mesh->hasPointNormals();

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(!m_ogreMesh->sharedVertexData)
    {
        m_ogreMesh->sharedVertexData = new ::Ogre::VertexData();
    }

    ::Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    size_t prevNumVertices            = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        prevNumVertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(!m_hasNormal && !_pointsOnly)
    {
        // Verify if mesh contains Tetra, Edge or Point
        // If not, generate normals
        auto cellItr        = _mesh->begin<data::iterator::ConstCellIterator>();
        const auto cellEnd  = _mesh->end<data::iterator::ConstCellIterator>();
        bool computeNormals = true;

        for( ; cellItr != cellEnd ; ++cellItr)
        {
            auto cellType = *cellItr->type;
            if(cellType == data::Mesh::CellType::EDGE || cellType == data::Mesh::CellType::TETRA
               || cellType == data::Mesh::CellType::POINT)
            {
                computeNormals = false;
                break;
            }
        }

        if(computeNormals)
        {
            geometry::data::Mesh::generatePointNormals(_mesh);
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
        ::Ogre::HardwareVertexBufferSharedPtr vbuf;
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                              ? ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                              : ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::Mesh
        ::Ogre::VertexDeclaration* declMain = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        declMain->removeAllElements();

        // 1st buffer
        declMain->addElement(m_binding[POSITION_NORMAL], offset, ::Ogre::VET_FLOAT3, ::Ogre::VES_POSITION);
        offset += ::Ogre::VertexElement::getTypeSize(::Ogre::VET_FLOAT3);

        if(m_hasNormal)
        {
            declMain->addElement(m_binding[POSITION_NORMAL], offset, ::Ogre::VET_FLOAT3, ::Ogre::VES_NORMAL);
            offset += ::Ogre::VertexElement::getTypeSize(::Ogre::VET_FLOAT3);
        }

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();
        vbuf = mgr.createVertexBuffer(offset, numVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vbuf);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = numVertices;
    }

    //------------------------------------------
    // Create indices arrays
    //------------------------------------------

    // 1 - Count number of primitives for each type
    auto cellItr       = _mesh->begin<data::iterator::ConstCellIterator>();
    const auto cellEnd = _mesh->end<data::iterator::ConstCellIterator>();
    unsigned int numIndices[s_numPrimitiveTypes];

    memset(numIndices, 0, sizeof(numIndices));

    for( ; cellItr != cellEnd ; ++cellItr)
    {
        auto cellType = _pointsOnly
                        ? data::Mesh::CellType::POINT
                        : *cellItr->type;
        if(cellType == data::Mesh::CellType::POINT)
        {
            numIndices[static_cast<unsigned int>(data::Mesh::CellType::POINT)] += 1;
        }
        else if(cellType == data::Mesh::CellType::EDGE)
        {
            numIndices[static_cast<unsigned int>(data::Mesh::CellType::EDGE)] += 2;
        }
        else if(cellType == data::Mesh::CellType::TRIANGLE)
        {
            numIndices[static_cast<unsigned int>(data::Mesh::CellType::TRIANGLE)] += 3;
        }
        else if(cellType == data::Mesh::CellType::QUAD)
        {
            numIndices[static_cast<unsigned int>(data::Mesh::CellType::QUAD)] += 4;
        }
        else if(cellType == data::Mesh::CellType::TETRA)
        {
            numIndices[static_cast<unsigned int>(data::Mesh::CellType::TETRA)] += 4;
        }
        else
        {
            SIGHT_ERROR("Unhandled cell type in Ogre mesh: " << static_cast<int>(cellType));
        }
    }

    // 2 - Create a submesh for each primitive type
    void* indexBuffer[s_numPrimitiveTypes];
    memset(indexBuffer, 0, sizeof(indexBuffer));

    const bool indices32Bits     = numVertices >= (1 << 16);
    const bool indicesPrev32Bits = prevNumVertices >= (1 << 16);
    const bool hasPrimitiveColor = _mesh->hasCellColors();

    {
        FW_PROFILE_AVG("REALLOC INDEX", 5);

        for(size_t i = 0 ; i < s_numPrimitiveTypes ; ++i)
        {
            const data::Mesh::CellType cellType = static_cast<data::Mesh::CellType>(i);

            if(numIndices[i] > 0)
            {
                if(!m_subMeshes[i])
                {
                    // Create one submesh
                    const std::string name = std::to_string(static_cast<int>(cellType));
                    if(cellType == data::Mesh::CellType::TRIANGLE)
                    {
                        if(hasPrimitiveColor)
                        {
                            // Use r2vb pipeline for per-primitive color
                            m_subMeshes[i] = m_r2vbMesh->createSubMesh(name);
                        }
                        else
                        {
                            m_subMeshes[i] = m_ogreMesh->createSubMesh(name);
                        }

                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_TRIANGLE_LIST;
                    }
                    else if(cellType == data::Mesh::CellType::EDGE)
                    {
                        m_subMeshes[i]                = m_ogreMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_LINE_LIST;
                    }
                    else if(cellType == data::Mesh::CellType::POINT)
                    {
                        m_subMeshes[i]                = m_ogreMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_POINT_LIST;
                    }
                    else if(cellType == data::Mesh::CellType::QUAD || cellType == data::Mesh::CellType::TETRA)
                    {
                        // Use r2vb pipeline to generate quads or tetrahedrons
                        m_subMeshes[i]                = m_r2vbMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_LINE_LIST;
                    }

                    m_subMeshes[i]->useSharedVertices     = true;
                    m_subMeshes[i]->indexData->indexStart = 0;
                }

                if(cellType != data::Mesh::CellType::POINT)
                {
                    ::Ogre::HardwareIndexBufferSharedPtr ibuf = m_subMeshes[i]->indexData->indexBuffer;

                    // Allocate index buffer of the requested number of vertices (ibufCount) if necessary
                    // We don't reallocate if we have more space than requested
                    bool createIndexBuffer = !ibuf;
                    if(ibuf)
                    {
                        // reallocate if new mesh has more indexes or IndexType change
                        createIndexBuffer = (ibuf->getNumIndexes() < numIndices[i])
                                            || (indicesPrev32Bits != indices32Bits);
                    }

                    if(createIndexBuffer)
                    {
                        ::Ogre::HardwareBuffer::Usage usage = m_isDynamic
                                                              ? ::Ogre::HardwareBuffer::
                                                              HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                                              :
                                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

                        ibuf = ::Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                            indices32Bits ? ::Ogre::HardwareIndexBuffer::IT_32BIT : ::Ogre::HardwareIndexBuffer::IT_16BIT,
                            numIndices[i],
                            usage
                        );

                        m_subMeshes[i]->indexData->indexBuffer = ibuf;
                    }

                    m_subMeshes[i]->indexData->indexCount = numIndices[i];
                    SIGHT_DEBUG("Index #" << m_subMeshes[i]->indexData->indexCount);

                    // Lock index data, we are going to write into it in the next loop
                    indexBuffer[i] = ibuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
                }
            }
            else
            {
                // Destroy the submesh if it has been created before - a submesh with 0 index would be invalid
                if(m_subMeshes[i])
                {
                    std::string name = std::to_string(static_cast<int>(cellType));
                    if((cellType == data::Mesh::CellType::TRIANGLE && hasPrimitiveColor)
                       || (cellType == data::Mesh::CellType::TETRA || cellType == data::Mesh::CellType::QUAD))
                    {
                        m_r2vbMesh->destroySubMesh(name);
                    }
                    else
                    {
                        m_ogreMesh->destroySubMesh(name);
                    }

                    m_subMeshes[i] = nullptr;
                }
            }
        }
    }

    // 3 - Copy indices data

    if(!_pointsOnly)
    {
        if(indices32Bits)
        {
            copyIndices<std::uint32_t>(
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::TRIANGLE)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::QUAD)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::EDGE)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::TETRA)],
                _mesh
            );
        }
        else
        {
            copyIndices<std::uint16_t>(
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::TRIANGLE)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::QUAD)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::EDGE)],
                indexBuffer[static_cast<std::uint8_t>(data::Mesh::CellType::TETRA)],
                _mesh
            );
        }
    }

    for(size_t i = 0 ; i < s_numPrimitiveTypes ; ++i)
    {
        if(numIndices[i] > 0 && m_subMeshes[i]->indexData->indexBuffer)
        {
            m_subMeshes[i]->indexData->indexBuffer->unlock();
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::updateMesh(const data::PointList::csptr& _pointList)
{
    auto points = _pointList->getPoints();

    /// The values in this table refer to vertices in the above table
    size_t uiNumVertices = points.size();
    SIGHT_DEBUG("Vertices #" << uiNumVertices);

    // Check if mesh attributes
    m_hasNormal = false;

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(!m_ogreMesh->sharedVertexData)
    {
        m_ogreMesh->sharedVertexData = new ::Ogre::VertexData();
    }

    ::Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    size_t uiPrevNumVertices          = 0;
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
        ::Ogre::HardwareVertexBufferSharedPtr vbuf;
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices)
                                              ? ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                              : ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        size_t offset = 0;

        // Create declaration (memory format) of vertex data based on data::Mesh
        ::Ogre::VertexDeclaration* declMain = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        declMain->removeAllElements();

        // 1st buffer
        declMain->addElement(m_binding[POSITION_NORMAL], offset, ::Ogre::VET_FLOAT3, ::Ogre::VES_POSITION);
        offset += ::Ogre::VertexElement::getTypeSize(::Ogre::VET_FLOAT3);

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();
        vbuf = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vbuf);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;
    }

    const size_t pointType = static_cast<size_t>(data::Mesh::CellType::POINT);
    if(m_subMeshes[pointType] == nullptr)
    {
        m_subMeshes[pointType] =
            m_ogreMesh->createSubMesh(std::to_string(pointType));
        m_subMeshes[pointType]->operationType = ::Ogre::RenderOperation::OT_POINT_LIST;
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::vector<R2VBRenderable*> > Mesh::updateR2VB(
    const data::Mesh::sptr& _mesh,
    ::Ogre::SceneManager& _sceneMgr,
    const std::string& _materialName,
    bool
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

    const bool hasPrimitiveColor = _mesh->hasCellColors();
    if((m_subMeshes[static_cast<unsigned int>(data::Mesh::CellType::QUAD)]
        || m_subMeshes[static_cast<unsigned int>(data::Mesh::CellType::TETRA)]) || hasPrimitiveColor)
    {
        if(!m_r2vbMesh->sharedVertexData)
        {
            m_r2vbMesh->sharedVertexData = new ::Ogre::VertexData();
        }

        *m_r2vbMesh->sharedVertexData->vertexBufferBinding = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
        *m_r2vbMesh->sharedVertexData->vertexDeclaration   = *m_ogreMesh->sharedVertexData->vertexDeclaration;
        m_r2vbMesh->sharedVertexData->vertexCount          = m_ogreMesh->sharedVertexData->vertexCount;

        m_r2vbMesh->_setBounds(m_ogreMesh->getBounds());
        m_r2vbMesh->_setBoundingSphereRadius(m_ogreMesh->getBoundingSphereRadius());

        // Add mesh to Ogre Root Scene Node if it doesn't exist yet
        if(!m_r2vbEntity)
        {
            m_r2vbEntity = _sceneMgr.createEntity(m_r2vbMesh);
            // Never attach it to the scene manager otherwise it will be computed all the time
            _sceneMgr.getRootSceneNode()->detachObject(m_r2vbEntity);
        }

        const size_t numSubEntities = m_r2vbEntity->getNumSubEntities();
        for(size_t i = 0 ; i < numSubEntities ; ++i)
        {
            const auto subEntity = m_r2vbEntity->getSubEntity(i);
            const auto subMesh   = subEntity->getSubMesh();

            const bool bQuad  = (subMesh == m_subMeshes[static_cast<std::uint8_t>(data::Mesh::CellType::QUAD)]);
            const bool bTetra = (subMesh == m_subMeshes[static_cast<std::uint8_t>(data::Mesh::CellType::TETRA)]);

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

        if(m_r2vbEntity)
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
    ::Ogre::VertexBufferBinding* bind          = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    ::Ogre::HardwareVertexBufferSharedPtr vbuf = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with data::Mesh
    const auto dumpLock = _mesh->lock();

    auto pointsItr       = _mesh->begin<data::iterator::ConstPointIterator>();
    const auto pointsEnd = _mesh->end<data::iterator::ConstPointIterator>();

    size_t uiStrideFloat = 3;
    if(m_hasNormal)
    {
        uiStrideFloat += 3;
    }

    typedef data::Mesh::PointValueType PointValueType;
    typedef data::Mesh::NormalValueType NormalValueType;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    PointValueType xMin = std::numeric_limits<PointValueType>::max();
    PointValueType yMin = std::numeric_limits<PointValueType>::max();
    PointValueType zMin = std::numeric_limits<PointValueType>::max();
    PointValueType xMax = std::numeric_limits<PointValueType>::lowest();
    PointValueType yMax = std::numeric_limits<PointValueType>::lowest();
    PointValueType zMax = std::numeric_limits<PointValueType>::lowest();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for( ; pointsItr != pointsEnd ; ++pointsItr)
        {
            const auto& pt0 = pointsItr->point->x;
            xMin = std::min(xMin, pt0);
            xMax = std::max(xMax, pt0);

            const auto& pt1 = pointsItr->point->y;
            yMin = std::min(yMin, pt1);
            yMax = std::max(yMax, pt1);

            const auto& pt2 = pointsItr->point->z;
            zMin = std::min(zMin, pt2);
            zMax = std::max(zMax, pt2);
        }
    }
    {
        pointsItr = _mesh->begin<data::iterator::ConstPointIterator>();
        PointValueType* __restrict pPos = static_cast<PointValueType*>(pVertex);
        FW_PROFILE_AVG("UPDATE POS AND NORMALS", 5);

        NormalValueType* __restrict pNormal = nullptr;

        if(m_hasNormal)
        {
            pNormal = static_cast<NormalValueType*>(pVertex) + 3;
        }

        for( ; pointsItr != pointsEnd ; ++pointsItr)
        {
            memcpy(pPos, pointsItr->point, 3 * sizeof(PointValueType));
            pPos += uiStrideFloat;

            if(m_hasNormal)
            {
                memcpy(pNormal, pointsItr->normal, 3 * sizeof(NormalValueType));
                pNormal += uiStrideFloat;
            }
        }
    }

    // Unlock vertex data
    vbuf->unlock();

    if(xMin < std::numeric_limits<PointValueType>::max()
       && yMin < std::numeric_limits<PointValueType>::max()
       && zMin < std::numeric_limits<PointValueType>::max()
       && xMax > std::numeric_limits<PointValueType>::lowest()
       && yMax > std::numeric_limits<PointValueType>::lowest()
       && zMax > std::numeric_limits<PointValueType>::lowest())
    {
        m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox(xMin, yMin, zMin, xMax, yMax, zMax));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = this->areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            m_ogreMesh->_setBoundingSphereRadius(
                ::Ogre::Math::Sqrt(
                    ::Ogre::Math::Sqr(xMax - xMin)
                    + ::Ogre::Math::Sqr(yMax - yMin)
                    + ::Ogre::Math::Sqr(zMax - zMin)
                ) / 2
            );
        }
        else
        {
            SIGHT_ERROR("Infinite or NaN values for the bounding box. Check the mesh validity.");

            // This silent the problem so there is no crash in Ogre
            m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateVertices(const data::PointList::csptr& _pointList)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    ::Ogre::VertexBufferBinding* bind          = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    ::Ogre::HardwareVertexBufferSharedPtr vbuf = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with data::Mesh
    const size_t uiStrideFloat = 3;

    typedef data::Point::PointCoordType PointType;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    PointType xMin = std::numeric_limits<PointType>::max();
    PointType yMin = std::numeric_limits<PointType>::max();
    PointType zMin = std::numeric_limits<PointType>::max();
    PointType xMax = std::numeric_limits<PointType>::lowest();
    PointType yMax = std::numeric_limits<PointType>::lowest();
    PointType zMax = std::numeric_limits<PointType>::lowest();

    const auto& points     = _pointList->getPoints();
    const size_t numPoints = points.size();

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for(size_t i = 0 ; i < numPoints ; ++i)
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
        float* __restrict pPos = static_cast<float*>(pVertex);
        FW_PROFILE_AVG("UPDATE POS", 5);
        for(size_t i = 0 ; i < numPoints ; ++i)
        {
            const auto point = points[i]->getCoord();
            pPos[0] = static_cast<float>(point[0]);
            pPos[1] = static_cast<float>(point[1]);
            pPos[2] = static_cast<float>(point[2]);

            pPos += uiStrideFloat;
        }
    }
    // Unlock vertex data
    vbuf->unlock();

    if(xMin < std::numeric_limits<PointType>::max()
       && yMin < std::numeric_limits<PointType>::max()
       && zMin < std::numeric_limits<PointType>::max()
       && xMax > std::numeric_limits<PointType>::lowest()
       && yMax > std::numeric_limits<PointType>::lowest()
       && zMax > std::numeric_limits<PointType>::lowest())
    {
        const float xMinF = static_cast<float>(xMin);
        const float yMinF = static_cast<float>(yMin);
        const float zMinF = static_cast<float>(zMin);
        const float xMaxF = static_cast<float>(xMax);
        const float yMaxF = static_cast<float>(yMax);
        const float zMaxF = static_cast<float>(zMax);

        m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox(xMinF, yMinF, zMinF, xMaxF, yMaxF, zMaxF));

        // Check again the bounds, since ogre may add some extent that could give infinite bounds
        const bool valid = this->areBoundsValid(m_ogreMesh);
        SIGHT_ASSERT("Invalid bounds found...", valid);

        if(valid)
        {
            const float xLenF = xMaxF - xMinF;
            const float yLenF = yMaxF - yMinF;
            const float zLenF = zMaxF - xMinF;

            m_ogreMesh->_setBoundingSphereRadius(
                ::Ogre::Math::Sqrt(
                    ::Ogre::Math::Sqr(xLenF)
                    + ::Ogre::Math::Sqr(yLenF)
                    + ::Ogre::Math::Sqr(zLenF)
                ) / 2.0f
            );
        }
        else
        {
            SIGHT_ERROR("Infinite or NaN values for the bounding box. Check the mesh validity.");

            // This silent the problem so there is no crash in Ogre
            m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox::EXTENT_NULL);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds(::Ogre::AxisAlignedBox::EXTENT_NULL);
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateColors(const data::Mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    ::Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SIGHT_ASSERT("Invalid vertex buffer binding", bind);

    const bool hasVertexColor    = _mesh->hasPointColors();
    const bool hasPrimitiveColor = _mesh->hasCellColors();

    // 1 - Initialization
    if(hasVertexColor)
    {
        bindLayer(_mesh, COLOUR, ::Ogre::VES_DIFFUSE, Ogre::VET_COLOUR);
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
        for(size_t i = 0 ; i < s_numPrimitiveTypes ; ++i)
        {
            if(m_subMeshes[i])
            {
                const data::Mesh::CellType cellType = static_cast<data::Mesh::CellType>(i);
                if(cellType == data::Mesh::CellType::TRIANGLE)
                {
                    numIndicesTotal += static_cast<unsigned int>(m_subMeshes[i]->indexData->indexCount / 3);
                }
                else
                {
                    numIndicesTotal += static_cast<unsigned int>(m_subMeshes[i]->indexData->indexCount >> 2);
                }
            }
        }

        if(!m_perPrimitiveColorTexture)
        {
            static std::uint64_t i = 0;
            m_perPrimitiveColorTextureName = "PerCellColorTexture_" + std::to_string(i++);
            m_perPrimitiveColorTexture     = ::Ogre::TextureManager::getSingleton().create(
                m_perPrimitiveColorTextureName,
                viz::scene3d::RESOURCE_GROUP,
                true
            );
        }

        const size_t width  = std::min(s_maxTextureSize, numIndicesTotal);
        const size_t height = static_cast<size_t>(std::floor(numIndicesTotal / s_maxTextureSize) + 1);

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
                ::Ogre::PF_BYTE_RGBA,
                ::Ogre::TEX_TYPE_2D,
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

    const auto lock = _mesh->lock();
    // 2 - Copy of vertices
    if(hasVertexColor)
    {
        // Source points
        ::Ogre::HardwareVertexBufferSharedPtr cbuf = bind->getBuffer(m_binding[COLOUR]);
        ::Ogre::RGBA* pColor                       =
            static_cast< ::Ogre::RGBA*>(cbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD));

        // Destination
        const std::uint8_t* colors = _mesh->getPointColorsBuffer();

        // Copy points
        const size_t nbComponents = _mesh->hasRGBPointColors() ? 3 : 4;
        viz::scene3d::helper::Mesh::copyColors(pColor, colors, _mesh->getNumberOfPoints(), nbComponents);

        cbuf->unlock();
    }

    if(hasPrimitiveColor)
    {
        // Source cells
        ::Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_perPrimitiveColorTexture->getBuffer();
        pixelBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        const ::Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        ::Ogre::RGBA* pColorDest         = reinterpret_cast< ::Ogre::RGBA*>(pixelBox.data);

        // Destination
        const std::uint8_t* colors = _mesh->getCellColorsBuffer();

        // Copy cells
        const size_t nbComponents = _mesh->hasRGBCellColors() ? 3 : 4;
        viz::scene3d::helper::Mesh::copyColors(pColorDest, colors, _mesh->getNumberOfCells(), nbComponents);

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
    m_hasUV = _mesh->hasPointTexCoords();

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if(m_hasUV)
    {
        bindLayer(_mesh, TEXCOORD, ::Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        ::Ogre::VertexBufferBinding* bind           = m_ogreMesh->sharedVertexData->vertexBufferBinding;
        ::Ogre::HardwareVertexBufferSharedPtr uvbuf = bind->getBuffer(m_binding[TEXCOORD]);
        void* pBuf                                  = uvbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        float* pUV                                  = static_cast<float*>(pBuf);

        auto itr = _mesh->begin<data::iterator::ConstPointIterator>();

        // Copy UV coordinates for each mesh point
        for(unsigned int i = 0 ; i < _mesh->getNumberOfPoints() ; ++i, ++itr)
        {
            pUV[2 * i]     = itr->tex->u;
            pUV[2 * i + 1] = itr->tex->v;
        }

        uvbuf->unlock();
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::clearMesh(::Ogre::SceneManager& _sceneMgr)
{
    // Destroy all the submeshes, but keep the two meshes alive
    for(size_t i = 0 ; i < s_numPrimitiveTypes ; ++i)
    {
        if(m_subMeshes[i])
        {
            const data::Mesh::CellType cellType = static_cast<data::Mesh::CellType>(i);
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

            m_subMeshes[i] = nullptr;
        }
    }

    for(auto r2vbObject : m_r2vbObject)
    {
        _sceneMgr.destroyMovableObject(r2vbObject.second);
    }

    m_r2vbObject.clear();

    if(m_r2vbEntity)
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

bool Mesh::hasColorLayerChanged(const data::Mesh::csptr& _mesh)
{
    const bool hasVertexColor    = _mesh->hasPointColors();
    const bool hasPrimitiveColor = _mesh->hasCellColors();

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

bool Mesh::areBoundsValid(const ::Ogre::MeshPtr& _ogreMesh)
{
    const ::Ogre::AxisAlignedBox& bounds = _ogreMesh->getBounds();
    const ::Ogre::Vector3& maximum       = bounds.getMaximum();
    const ::Ogre::Vector3& minimum       = bounds.getMinimum();

    return !maximum.isNaN() && !minimum.isNaN()
           && !std::isinf(maximum[0]) && !std::isinf(maximum[1]) && !std::isinf(maximum[2])
           && !std::isinf(minimum[0]) && !std::isinf(minimum[1]) && !std::isinf(minimum[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
