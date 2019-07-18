/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "fwRenderOgre/Mesh.hpp"

#include "fwRenderOgre/R2VBRenderable.hpp"
#include <fwRenderOgre/helper/Mesh.hpp>
#include <fwRenderOgre/Utils.hpp>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwDataTools/helper/MeshGetter.hpp>
#include <fwDataTools/Mesh.hpp>

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

namespace fwRenderOgre
{

const unsigned int Mesh::s_maxTextureSize;

//-----------------------------------------------------------------------------

template <typename T>
void copyIndices(void* _pTriangles, void* _pQuads, void* _pEdges, void* _pTetras,
                 ::fwDataTools::helper::MeshGetter& _meshHelper, size_t _uiNumCells)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* pTriangles = static_cast<T*>(_pTriangles);
    T* pQuads     = static_cast<T*>(_pQuads);
    T* pEdges     = static_cast<T*>(_pEdges);
    T* pTetras    = static_cast<T*>(_pTetras);

    ::fwData::Mesh::ConstCellDataMultiArrayType cells                  = _meshHelper.getCellData();
    ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType cellDataOffsets = _meshHelper.getCellDataOffsets();
    ::fwData::Mesh::ConstCellTypesMultiArrayType cellsType             = _meshHelper.getCellTypes();

    for (unsigned int i = 0; i < _uiNumCells; ++i)
    {
        long offset = static_cast<long>(cellDataOffsets[static_cast<int>(i)]);
        if ( cellsType[static_cast<int>(i)] == ::fwData::Mesh::TRIANGLE )
        {
            *pTriangles++ = static_cast<T>(cells[offset]);
            *pTriangles++ = static_cast<T>(cells[offset + 1]);
            *pTriangles++ = static_cast<T>(cells[offset + 2]);
        }
        else if ( cellsType[static_cast<int>(i)] == ::fwData::Mesh::QUAD )
        {
            *pQuads++ = static_cast<T>(cells[offset]);
            *pQuads++ = static_cast<T>(cells[offset + 1]);
            *pQuads++ = static_cast<T>(cells[offset + 2]);
            *pQuads++ = static_cast<T>(cells[offset + 3]);
        }
        else if ( cellsType[static_cast<int>(i)] == ::fwData::Mesh::EDGE )
        {
            *pEdges++ = static_cast<T>(cells[offset]);
            *pEdges++ = static_cast<T>(cells[offset + 1]);
        }
        else if ( cellsType[static_cast<int>(i)] == ::fwData::Mesh::TETRA )
        {
            *pTetras++ = static_cast<T>(cells[offset]);
            *pTetras++ = static_cast<T>(cells[offset + 1]);
            *pTetras++ = static_cast<T>(cells[offset + 2]);
            *pTetras++ = static_cast<T>(cells[offset + 3]);
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
    m_ogreMesh = meshMgr.createManual(meshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    // Create Mesh Data Structure for r2vb input
    // We could create it only when necessary, but for now it is simpler to create it every time
    const auto r2vbMeshName = _name + "_R2VBMesh";
    m_r2vbMesh = meshMgr.createManual(r2vbMeshName,
                                      ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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

void Mesh::bindLayer(const ::fwData::Mesh::csptr& _mesh, BufferBinding _binding,
                     ::Ogre::VertexElementSemantic _semantic, ::Ogre::VertexElementType _type)
{
    ::Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SLM_ASSERT("Invalid vertex buffer binding", bind);

    ::Ogre::VertexDeclaration* vtxDecl = m_ogreMesh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;

    if(!vtxDecl->findElementBySemantic(_semantic))
    {
        m_binding[_binding] = static_cast<unsigned short>(bind->getBindings().size());

        vtxDecl->addElement(m_binding[_binding], offset, _type, _semantic);
        offset += ::Ogre::VertexElement::getTypeSize(_type);
    }

    ::Ogre::HardwareVertexBufferSharedPtr cbuf;

    const size_t uiNumVertices = _mesh->getNumberOfPoints();
    size_t uiPrevNumVertices   = 0;
    if(bind->isBufferBound(m_binding[_binding]))
    {
        cbuf              = bind->getBuffer(m_binding[_binding]);
        uiPrevNumVertices = cbuf->getNumVertices();
    }

    if(!bind->isBufferBound(m_binding[_binding]) || uiPrevNumVertices < uiNumVertices )
    {
        FW_PROFILE_AVG("REALLOC LAYER", 5);

        // Allocate color buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices) ?
                                              ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE :
                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();
        cbuf                               = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
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

void Mesh::updateMesh(const ::fwData::Mesh::sptr& _mesh, bool _pointsOnly)
{
    ::fwDataTools::helper::MeshGetter meshHelper(_mesh);

    /// The values in this table refer to vertices in the above table
    const size_t numVertices = _mesh->getNumberOfPoints();
    OSLM_DEBUG("Vertices #" << numVertices);

    // Check if the mesh has normals - we assume we should have as many normals as points
    // If this is not the case, normals will be ignored or regenerated if needed and if the number of vertices changed
    m_hasNormal = false;
    if( auto normals = _mesh->getPointNormalsArray() )
    {
        const auto numNormals = normals->getNumberOfElements() / normals->getNumberOfComponents();
        if(numNormals >= numVertices)
        {
            m_hasNormal = true;
        }
    }

    //------------------------------------------
    // Create vertex arrays
    //------------------------------------------

    // Create vertex data structure for all vertices shared between submeshes
    if(!m_ogreMesh->sharedVertexData)
    {
        m_ogreMesh->sharedVertexData = new ::Ogre::VertexData();
    }

    ::Ogre::VertexBufferBinding& bind = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
    size_t prevNumVertices = 0;
    if(bind.isBufferBound(m_binding[POSITION_NORMAL]))
    {
        prevNumVertices = bind.getBuffer(m_binding[POSITION_NORMAL])->getNumVertices();
    }

    if(prevNumVertices < numVertices)
    {
        FW_PROFILE("REALLOC MESH");

        if(!m_hasNormal && !_pointsOnly)
        {
            // Verify if mesh contains Tetra, Edge or Point
            // If not, generate normals
            ::fwData::Mesh::ConstCellTypesMultiArrayType cellsType = meshHelper.getCellTypes();
            bool computeNormals = true;

            for(unsigned int i = 0; i < cellsType.size(); ++i)
            {
                auto cellType = cellsType[static_cast<int>(i)];
                if(cellType == ::fwData::Mesh::EDGE || cellType == ::fwData::Mesh::TETRA
                   || cellType == ::fwData::Mesh::POINT)
                {
                    computeNormals = false;
                    break;
                }
            }

            if(computeNormals)
            {
                ::fwDataTools::Mesh::generatePointNormals(_mesh);
                m_hasNormal = true;
            }
        }

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = numVertices;

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        ::Ogre::HardwareVertexBufferSharedPtr vbuf;
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices) ?
                                              ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE :
                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        size_t offset = 0;

        // Create declaration (memory format) of vertex data based on ::fwData::Mesh
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
        vbuf                               = mgr.createVertexBuffer(offset, numVertices, usage, false);
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
    ::fwData::Mesh::ConstCellTypesMultiArrayType cellsType = meshHelper.getCellTypes();
    unsigned int numIndices[ s_numPrimitiveTypes ];

    memset(numIndices, 0, sizeof(numIndices));

    for(unsigned int i = 0; i < cellsType.size(); ++i)
    {
        auto cellType = _pointsOnly ?
                        ::fwData::Mesh::POINT :
                        static_cast< ::fwData::Mesh::CellTypesEnum>(cellsType[static_cast<int>(i)]);
        if(cellType == ::fwData::Mesh::POINT)
        {
            numIndices[::fwData::Mesh::POINT] += 1;
        }
        else if(cellType == ::fwData::Mesh::EDGE)
        {
            numIndices[::fwData::Mesh::EDGE] += 2;
        }
        else if(cellType == ::fwData::Mesh::TRIANGLE)
        {
            numIndices[::fwData::Mesh::TRIANGLE] += 3;
        }
        else if(cellType == ::fwData::Mesh::QUAD)
        {
            numIndices[::fwData::Mesh::QUAD] += 4;
        }
        else if(cellType == ::fwData::Mesh::TETRA)
        {
            numIndices[::fwData::Mesh::TETRA] += 4;
        }
        else
        {
            OSLM_ERROR("Unhandled cell type in Ogre mesh: " << static_cast<int>(cellType));
        }
    }

    // 2 - Create a submesh for each primitive type
    void* indexBuffer[ s_numPrimitiveTypes ];
    memset(indexBuffer, 0, sizeof(indexBuffer));

    const bool indices32Bits     = numVertices >= (1 << 16);
    const bool indicesPrev32Bits = prevNumVertices >= (1 << 16);
    const bool hasPrimitiveColor = (_mesh->getCellColorsArray() != nullptr);

    {
        FW_PROFILE_AVG("REALLOC INDEX", 5);

        for(size_t i = 0; i < s_numPrimitiveTypes; ++i)
        {
            const ::fwData::Mesh::CellTypesEnum cellType = static_cast< ::fwData::Mesh::CellTypesEnum>(i);

            if(numIndices[i] > 0)
            {
                if(!m_subMeshes[i])
                {
                    // Create one submesh
                    const std::string name = std::to_string(cellType);
                    if ( cellType == ::fwData::Mesh::TRIANGLE )
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
                    else if(cellType == ::fwData::Mesh::EDGE )
                    {
                        m_subMeshes[i]                = m_ogreMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_LINE_LIST;
                    }
                    else if(cellType == ::fwData::Mesh::POINT )
                    {
                        m_subMeshes[i]                = m_ogreMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_POINT_LIST;
                    }
                    else if(cellType == ::fwData::Mesh::QUAD || cellType == ::fwData::Mesh::TETRA )
                    {
                        // Use r2vb pipeline to generate quads or tetrahedrons
                        m_subMeshes[i]                = m_r2vbMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_LINE_LIST;
                    }

                    m_subMeshes[i]->useSharedVertices     = true;
                    m_subMeshes[i]->indexData->indexStart = 0;
                }

                if(cellType != ::fwData::Mesh::POINT)
                {
                    ::Ogre::HardwareIndexBufferSharedPtr ibuf = m_subMeshes[i]->indexData->indexBuffer;

                    // Allocate index buffer of the requested number of vertices (ibufCount) if necessary
                    // We don't reallocate if we have more space than requested
                    bool createIndexBuffer = !ibuf;
                    if( ibuf)
                    {
                        // reallocate if new mesh has more indexes or IndexType change
                        createIndexBuffer = (ibuf->getNumIndexes() < numIndices[i]) ||
                                            (indicesPrev32Bits != indices32Bits);
                    }
                    if(createIndexBuffer)
                    {
                        ::Ogre::HardwareBuffer::Usage usage = m_isDynamic ?
                                                              ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
                                                              :
                                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

                        ibuf = ::Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                            indices32Bits ? ::Ogre::HardwareIndexBuffer::IT_32BIT : ::Ogre::HardwareIndexBuffer::IT_16BIT,
                            numIndices[i], usage);

                        m_subMeshes[i]->indexData->indexBuffer = ibuf;
                    }
                    m_subMeshes[i]->indexData->indexCount = numIndices[i];
                    OSLM_DEBUG("Index #" << m_subMeshes[i]->indexData->indexCount );

                    // Lock index data, we are going to write into it in the next loop
                    indexBuffer[i] = ibuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
                }
            }
            else
            {
                // Destroy the submesh if it has been created before - a submesh with 0 index would be invalid
                if(m_subMeshes[i])
                {
                    std::string name = std::to_string(cellType);
                    if ( (cellType == ::fwData::Mesh::TRIANGLE && hasPrimitiveColor) ||
                         (cellType == ::fwData::Mesh::TETRA || cellType == ::fwData::Mesh::QUAD))
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
            copyIndices< std::uint32_t >( indexBuffer[::fwData::Mesh::TRIANGLE], indexBuffer[::fwData::Mesh::QUAD],
                                          indexBuffer[::fwData::Mesh::EDGE], indexBuffer[::fwData::Mesh::TETRA],
                                          meshHelper, _mesh->getNumberOfCells() );
        }
        else
        {
            copyIndices< std::uint16_t >( indexBuffer[::fwData::Mesh::TRIANGLE], indexBuffer[::fwData::Mesh::QUAD],
                                          indexBuffer[::fwData::Mesh::EDGE], indexBuffer[::fwData::Mesh::TETRA],
                                          meshHelper, _mesh->getNumberOfCells() );
        }
    }

    for(size_t i = 0; i < s_numPrimitiveTypes; ++i)
    {
        if(numIndices[i] > 0 && m_subMeshes[i]->indexData->indexBuffer)
        {
            m_subMeshes[i]->indexData->indexBuffer->unlock();
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::updateMesh(const ::fwData::PointList::csptr& _pointList)
{
    auto points = _pointList->getPoints();

    /// The values in this table refer to vertices in the above table
    size_t uiNumVertices = points.size();
    OSLM_DEBUG("Vertices #" << uiNumVertices);

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
    size_t uiPrevNumVertices = 0;
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
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices) ?
                                              ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE :
                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        size_t offset = 0;

        // Create declaration (memory format) of vertex data based on ::fwData::Mesh
        ::Ogre::VertexDeclaration* declMain = m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Clear if necessary
        declMain->removeAllElements();

        // 1st buffer
        declMain->addElement(m_binding[POSITION_NORMAL], offset, ::Ogre::VET_FLOAT3, ::Ogre::VES_POSITION);
        offset += ::Ogre::VertexElement::getTypeSize(::Ogre::VET_FLOAT3);

        // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();
        vbuf                               = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vbuf);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;
    }

    if(m_subMeshes[::fwData::Mesh::POINT] == nullptr)
    {
        m_subMeshes[::fwData::Mesh::POINT] =
            m_ogreMesh->createSubMesh(std::to_string(::fwData::Mesh::POINT));
        m_subMeshes[::fwData::Mesh::POINT]->operationType = ::Ogre::RenderOperation::OT_POINT_LIST;
    }
}

//------------------------------------------------------------------------------

std::pair<bool, std::vector<R2VBRenderable*> > Mesh::updateR2VB(const ::fwData::Mesh::sptr& _mesh,
                                                                ::Ogre::SceneManager& _sceneMgr,
                                                                const std::string& _materialName, bool _hasTexture)
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

    const bool hasPrimitiveColor = (_mesh->getCellColorsArray() != nullptr);
    if( (m_subMeshes[::fwData::Mesh::QUAD] || m_subMeshes[::fwData::Mesh::TETRA]) || hasPrimitiveColor)
    {
        if(!m_r2vbMesh->sharedVertexData)
        {
            m_r2vbMesh->sharedVertexData = new ::Ogre::VertexData();
        }

        *m_r2vbMesh->sharedVertexData->vertexBufferBinding = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
        *m_r2vbMesh->sharedVertexData->vertexDeclaration   = *m_ogreMesh->sharedVertexData->vertexDeclaration;
        m_r2vbMesh->sharedVertexData->vertexCount          = m_ogreMesh->sharedVertexData->vertexCount;

        m_r2vbMesh->_setBounds( m_ogreMesh->getBounds() );
        m_r2vbMesh->_setBoundingSphereRadius( m_ogreMesh->getBoundingSphereRadius() );

        // Add mesh to Ogre Root Scene Node if it doesn't exist yet
        if(!m_r2vbEntity)
        {
            m_r2vbEntity = _sceneMgr.createEntity(m_r2vbMesh);
            // Never attach it to the scene manager otherwise it will be computed all the time
            _sceneMgr.getRootSceneNode()->detachObject(m_r2vbEntity);
        }

        const size_t numSubEntities = m_r2vbEntity->getNumSubEntities();
        for(size_t i = 0; i < numSubEntities; ++i)
        {
            const auto subEntity = m_r2vbEntity->getSubEntity(i);
            const auto subMesh   = subEntity->getSubMesh();

            const bool bQuad  = (subMesh == m_subMeshes[::fwData::Mesh::QUAD]);
            const bool bTetra = (subMesh == m_subMeshes[::fwData::Mesh::TETRA]);

            const ::fwData::Mesh::CellTypesEnum cellType = bQuad ? ::fwData::Mesh::QUAD :
                                                           bTetra ? ::fwData::Mesh::TETRA :
                                                           ::fwData::Mesh::TRIANGLE;

            if(m_r2vbObject.find(cellType) == m_r2vbObject.end())
            {
                const std::string name           = std::to_string(cellType);
                const std::string r2vbObjectName = m_ogreMesh->getName() + "_r2vbObject_" + name;
                m_r2vbObject[cellType] = ::fwRenderOgre::R2VBRenderable::New(r2vbObjectName, subEntity, &_sceneMgr,
                                                                             cellType, _materialName);
            }

            m_r2vbObject[cellType]->setOutputSettings(static_cast<unsigned int>(subMesh->indexData->indexCount),
                                                      m_hasPrimitiveColor || m_hasVertexColor,
                                                      m_hasUV);

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

void Mesh::updateVertices(const ::fwData::Mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    ::Ogre::VertexBufferBinding* bind          = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    ::Ogre::HardwareVertexBufferSharedPtr vbuf = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with ::fwData::Mesh
    ::fwDataTools::helper::MeshGetter meshHelper(_mesh);
    ::fwData::Mesh::ConstPointsMultiArrayType points = meshHelper.getPoints();

    size_t uiStrideFloat = 3;
    if(m_hasNormal)
    {
        uiStrideFloat += 3;
    }

    typedef ::fwData::Mesh::PointValueType PointValueType;
    typedef ::fwData::Mesh::NormalValueType NormalValueType;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    PointValueType xMin = std::numeric_limits<PointValueType>::max();
    PointValueType yMin = std::numeric_limits<PointValueType>::max();
    PointValueType zMin = std::numeric_limits<PointValueType>::max();
    PointValueType xMax = std::numeric_limits<PointValueType>::lowest();
    PointValueType yMax = std::numeric_limits<PointValueType>::lowest();
    PointValueType zMax = std::numeric_limits<PointValueType>::lowest();

    const unsigned int numPoints = static_cast<unsigned int>(_mesh->getNumberOfPoints());

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            const auto& pt0 = points[static_cast<int>(i)][0];
            xMin = std::min(xMin, pt0);
            xMax = std::max(xMax, pt0);

            const auto& pt1 = points[static_cast<int>(i)][1];
            yMin = std::min(yMin, pt1);
            yMax = std::max(yMax, pt1);

            const auto& pt2 = points[static_cast<int>(i)][2];
            zMin = std::min(zMin, pt2);
            zMax = std::max(zMax, pt2);
        }
    }
    {
        PointValueType* __restrict pPos = static_cast< PointValueType* >( pVertex );
        FW_PROFILE_AVG("UPDATE POS", 5);
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            memcpy(pPos, &points[static_cast<int>(i)][0], 3 * sizeof(PointValueType) );
            pPos += uiStrideFloat;
        }
    }

    if(m_hasNormal)
    {
        FW_PROFILE_AVG("UPDATE NORMALS", 5);
        ::fwData::Mesh::ConstPointNormalsMultiArrayType normals = meshHelper.getPointNormals();
        NormalValueType* __restrict pNormal = static_cast< NormalValueType* >( pVertex ) + 3;
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            memcpy(pNormal, &normals[static_cast<int>(i)][0], 3 * sizeof(NormalValueType) );
            pNormal += uiStrideFloat;
        }
    }

    // Unlock vertex data
    vbuf->unlock();

    if(xMin < std::numeric_limits<PointValueType>::max() &&
       yMin < std::numeric_limits<PointValueType>::max() &&
       zMin < std::numeric_limits<PointValueType>::max() &&
       xMax > std::numeric_limits<PointValueType>::lowest() &&
       yMax > std::numeric_limits<PointValueType>::lowest() &&
       zMax > std::numeric_limits<PointValueType>::lowest())
    {
        m_ogreMesh->_setBounds( ::Ogre::AxisAlignedBox( xMin, yMin, zMin, xMax, yMax, zMax) );

        SLM_ASSERT("Infinite bounds found...",
                   !isinf(m_ogreMesh->getBounds().getMaximum()[0])
                   && !isinf(m_ogreMesh->getBounds().getMaximum()[1])
                   && !isinf(m_ogreMesh->getBounds().getMaximum()[2])
                   && !isinf(m_ogreMesh->getBounds().getMinimum()[0])
                   && !isinf(m_ogreMesh->getBounds().getMinimum()[1])
                   && !isinf(m_ogreMesh->getBounds().getMinimum()[2])
                   && !isnan(m_ogreMesh->getBounds().getMaximum()[0])
                   && !isnan(m_ogreMesh->getBounds().getMaximum()[1])
                   && !isnan(m_ogreMesh->getBounds().getMaximum()[2])
                   && !isnan(m_ogreMesh->getBounds().getMinimum()[0])
                   && !isnan(m_ogreMesh->getBounds().getMinimum()[1])
                   && !isnan(m_ogreMesh->getBounds().getMinimum()[2]));

        const auto bounds  = m_ogreMesh->getBounds();
        const auto maximum = bounds.getMaximum();
        const auto minimum = bounds.getMinimum();

        if(isinf(maximum[0]) || isnan(maximum[0])
           || isinf(maximum[1]) || isnan(maximum[1])
           || isinf(maximum[2]) || isnan(maximum[2])
           || isinf(minimum[0]) || isnan(minimum[0])
           || isinf(minimum[1]) || isnan(minimum[1])
           || isinf(minimum[2]) || isnan(minimum[2]))
        {
            SLM_ERROR("Infinite or NaN values for the bounding box. Check the mesh validity.");

            // This silent the problem so there is no crash in Ogre
            m_ogreMesh->_setBounds( ::Ogre::AxisAlignedBox::EXTENT_NULL );
        }
        else
        {
            m_ogreMesh->_setBoundingSphereRadius( ::Ogre::Math::Sqrt( ::Ogre::Math::Sqr(xMax - xMin) +
                                                                      ::Ogre::Math::Sqr(yMax - yMin) +
                                                                      ::Ogre::Math::Sqr(zMax - zMin)) /2);
        }
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds( ::Ogre::AxisAlignedBox::EXTENT_NULL );
    }

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateVertices(const ::fwData::PointList::csptr& _pointList)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    ::Ogre::VertexBufferBinding* bind          = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    ::Ogre::HardwareVertexBufferSharedPtr vbuf = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the vertex data to the GPU
    void* pVertex = vbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with ::fwData::Mesh
    const size_t uiStrideFloat = 3;

    typedef ::fwData::Point::PointCoordType PointType;

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
        for (size_t i = 0; i < numPoints; ++i)
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
        float* __restrict pPos = static_cast< float* >( pVertex );
        FW_PROFILE_AVG("UPDATE POS", 5);
        for (size_t i = 0; i < numPoints; ++i)
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

    if(xMin < std::numeric_limits<PointType>::max() &&
       yMin < std::numeric_limits<PointType>::max() &&
       zMin < std::numeric_limits<PointType>::max() &&
       xMax > std::numeric_limits<PointType>::lowest() &&
       yMax > std::numeric_limits<PointType>::lowest() &&
       zMax > std::numeric_limits<PointType>::lowest())
    {
        m_ogreMesh->_setBounds(
            ::Ogre::AxisAlignedBox(static_cast<float>(xMin), static_cast<float>(yMin), static_cast<float>(zMin),
                                   static_cast<float>(xMax), static_cast<float>(yMax), static_cast<float>(zMax)) );
    }
    else
    {
        // An extent was not found or is NaN
        m_ogreMesh->_setBounds( ::Ogre::AxisAlignedBox::EXTENT_NULL );
    }
    m_ogreMesh->_setBoundingSphereRadius( ::Ogre::Math::Sqrt( ::Ogre::Math::Sqr(static_cast<float>(xMax - xMin)) +
                                                              ::Ogre::Math::Sqr(static_cast<float>(yMax - yMin)) +
                                                              ::Ogre::Math::Sqr(static_cast<float>(zMax - zMin)))* .5f);

    /// Notify Mesh object that it has been modified
    m_ogreMesh->load();
}

//-----------------------------------------------------------------------------

void Mesh::updateColors(const ::fwData::Mesh::csptr& _mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    ::Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SLM_ASSERT("Invalid vertex buffer binding", bind);

    const bool hasVertexColor    = (_mesh->getPointColorsArray() != nullptr);
    const bool hasPrimitiveColor = (_mesh->getCellColorsArray() != nullptr);

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
        for(size_t i = 0; i < s_numPrimitiveTypes; ++i)
        {
            if(m_subMeshes[i])
            {
                const ::fwData::Mesh::CellTypesEnum cellType = static_cast< ::fwData::Mesh::CellTypesEnum>(i);
                if ( cellType == ::fwData::Mesh::TRIANGLE)
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
                m_perPrimitiveColorTextureName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
        }

        const size_t width  = std::min(s_maxTextureSize, numIndicesTotal);
        const size_t height = static_cast<size_t>( std::floor(numIndicesTotal / s_maxTextureSize) + 1 );

        if(m_perPrimitiveColorTexture->getWidth() != width || m_perPrimitiveColorTexture->getHeight() != height )
        {
            FW_PROFILE_AVG("REALLOC COLORS_CELL", 5);

            // It would be better to use PF_BYTE_RGB when we have 3 components but for some reason it doesn't work
            // Probably something related to alignment or a bug in Ogre
            ::fwRenderOgre::Utils::allocateTexture(m_perPrimitiveColorTexture.get(), width, height, 1,
                                                   ::Ogre::PF_BYTE_RGBA, ::Ogre::TEX_TYPE_2D,
                                                   (m_isDynamic || m_isDynamicVertices));

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

    // 2 - Copy of vertices
    if (hasVertexColor)
    {
        ::fwDataTools::helper::MeshGetter meshHelper(_mesh);

        // Source points
        ::Ogre::HardwareVertexBufferSharedPtr cbuf = bind->getBuffer(m_binding[COLOUR]);
        ::Ogre::RGBA* pColor                       =
            static_cast< ::Ogre::RGBA* >( cbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD) );

        // Destination
        const auto colors           = meshHelper.getPointColors();
        const std::uint8_t* icolors = reinterpret_cast<const std::uint8_t*>(colors.data());

        // Copy points
        ::fwRenderOgre::helper::Mesh::copyColors(pColor, icolors, _mesh->getNumberOfPoints(), colors.shape()[1]);

        cbuf->unlock();
    }

    if(hasPrimitiveColor)
    {
        ::fwDataTools::helper::MeshGetter meshHelper(_mesh);

        // Source cells
        ::Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_perPrimitiveColorTexture->getBuffer();
        pixelBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        const ::Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        ::Ogre::RGBA* pColorDest = reinterpret_cast< ::Ogre::RGBA* >( pixelBox.data );

        // Destination
        auto colors                 = meshHelper.getCellColors();
        const std::uint8_t* icolors = reinterpret_cast<const std::uint8_t*>(colors.data());

        // Copy cells
        ::fwRenderOgre::helper::Mesh::copyColors(pColorDest, icolors, _mesh->getNumberOfCells(), colors.shape()[1]);

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

void Mesh::updateTexCoords(const ::fwData::Mesh::csptr& _mesh)
{
    m_hasUV = (_mesh->getPointTexCoordsArray() != nullptr);

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if (m_hasUV)
    {
        bindLayer(_mesh, TEXCOORD, ::Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        ::fwDataTools::helper::MeshGetter meshHelper(_mesh);

        ::Ogre::VertexBufferBinding* bind           = m_ogreMesh->sharedVertexData->vertexBufferBinding;
        ::Ogre::HardwareVertexBufferSharedPtr uvbuf = bind->getBuffer(m_binding[TEXCOORD]);
        void* pBuf = uvbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        float* pUV = static_cast< float* >( pBuf );

        ::fwData::Mesh::ConstPointTexCoordsMultiArrayType uvCoord = meshHelper.getPointTexCoords();

        // Copy UV coordinates for each mesh point
        for (unsigned int i = 0; i < _mesh->getNumberOfPoints(); ++i)
        {
            pUV[2 * i]     = uvCoord[static_cast<int>(i)][0];
            pUV[2 * i + 1] = uvCoord[static_cast<int>(i)][1];
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
    for(size_t i = 0; i < s_numPrimitiveTypes; ++i)
    {
        if(m_subMeshes[i])
        {
            const ::fwData::Mesh::CellTypesEnum cellType = static_cast< ::fwData::Mesh::CellTypesEnum>(i);
            const std::string name                       = std::to_string(cellType);
            if ( (cellType == ::fwData::Mesh::TRIANGLE && m_hasPrimitiveColor) ||
                 (cellType == ::fwData::Mesh::TETRA || cellType == ::fwData::Mesh::QUAD))
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

bool Mesh::hasColorLayerChanged(const fwData::Mesh::csptr& _mesh)
{
    const bool hasVertexColor    = (_mesh->getPointColorsArray() != nullptr);
    const bool hasPrimitiveColor = (_mesh->getCellColorsArray() != nullptr);

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

} // namespace fwRenderOgre
