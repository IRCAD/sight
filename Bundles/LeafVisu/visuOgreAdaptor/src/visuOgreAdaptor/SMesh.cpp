/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SMesh.hpp"

#define FW_PROFILING_DISABLED

#include <fwCore/Profiling.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwDataTools/Mesh.hpp>

#include <fwRenderOgre/helper/Mesh.hpp>
#include <fwRenderOgre/R2VBRenderable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include "visuOgreAdaptor/SMaterial.hpp"

#include <cstdint>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SMesh, ::fwData::Mesh );

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

const unsigned int SMesh::s_maxTextureSize;

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT       = "updateVisibility";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_MESH_SLOT             = "modifyMesh";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_COLORS_SLOT           = "modifyColors";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_POINT_TEX_COORDS_SLOT = "modifyTexCoords";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_VERTICES_SLOT         = "modifyVertices";

//-----------------------------------------------------------------------------

template <typename T>
void copyIndices(void* _pTriangles, void* _pQuads, void* _pEdges, void* _pTetras,
                 ::fwDataTools::helper::Mesh& _meshHelper, size_t uiNumCells)
{
    FW_PROFILE_AVG("copyIndices", 5);

    T* pTriangles = static_cast<T*>(_pTriangles);
    T* pQuads     = static_cast<T*>(_pQuads);
    T* pEdges     = static_cast<T*>(_pEdges);
    T* pTetras    = static_cast<T*>(_pTetras);

    ::fwData::Mesh::CellDataMultiArrayType cells                  = _meshHelper.getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = _meshHelper.getCellDataOffsets();
    ::fwData::Mesh::CellTypesMultiArrayType cellsType             = _meshHelper.getCellTypes();

    for (unsigned int i = 0; i < uiNumCells; ++i)
    {
        long offset = static_cast<long>(cellDataOffsets[i]);
        if ( cellsType[i] == ::fwData::Mesh::TRIANGLE )
        {
            *pTriangles++ = static_cast<T>(cells[offset]);
            *pTriangles++ = static_cast<T>(cells[offset + 1]);
            *pTriangles++ = static_cast<T>(cells[offset + 2]);
        }
        else if ( cellsType[i] == ::fwData::Mesh::QUAD )
        {
            *pQuads++ = static_cast<T>(cells[offset]);
            *pQuads++ = static_cast<T>(cells[offset + 1]);
            *pQuads++ = static_cast<T>(cells[offset + 2]);
            *pQuads++ = static_cast<T>(cells[offset + 3]);
        }
        else if ( cellsType[i] == ::fwData::Mesh::EDGE )
        {
            *pEdges++ = static_cast<T>(cells[offset]);
            *pEdges++ = static_cast<T>(cells[offset + 1]);
        }
        else if ( cellsType[i] == ::fwData::Mesh::TETRA )
        {
            *pTetras++ = static_cast<T>(cells[offset]);
            *pTetras++ = static_cast<T>(cells[offset + 1]);
            *pTetras++ = static_cast<T>(cells[offset + 2]);
            *pTetras++ = static_cast<T>(cells[offset + 3]);
        }
    }
}

//-----------------------------------------------------------------------------

SMesh::SMesh() throw() :
    m_autoResetCamera(true),
    m_entity(nullptr),
    m_materialTemplateName(SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_hasNormal(false),
    m_hasVertexColor(false),
    m_hasPrimitiveColor(false),
    m_isDynamic(false),
    m_isDynamicVertices(false),
    m_hasUV(false),
    m_isReconstructionManaged(false),
    m_useNewMaterialAdaptor(false),
    m_isVisible(true),
    m_r2vbEntity(nullptr)
{
    m_material = ::fwData::Material::New();

    newSlot(s_UPDATE_VISIBILITY_SLOT, &SMesh::updateVisibility, this);
    newSlot(s_MODIFY_MESH_SLOT, &SMesh::modifyMesh, this);
    newSlot(s_MODIFY_COLORS_SLOT, &SMesh::modifyPointColors, this);
    newSlot(s_MODIFY_POINT_TEX_COORDS_SLOT, &SMesh::modifyTexCoords, this);
    newSlot(s_MODIFY_VERTICES_SLOT, &SMesh::modifyVertices, this);

    m_ogreMesh.setNull();
    m_r2vbMesh.setNull();
    m_perPrimitiveColorTexture.setNull();

    memset(m_subMeshes, 0, sizeof(m_subMeshes));

    m_binding[POSITION_NORMAL] = 0;
    m_binding[COLOUR]          = 0xFFFF;
    m_binding[TEXCOORD]        = 0xFFFF;
}

//-----------------------------------------------------------------------------

SMesh::~SMesh() throw()
{
    if(m_entity)
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void visuOgreAdaptor::SMesh::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    if(m_entity)
    {
        m_entity->setVisible(isVisible);

        if(m_r2vbEntity)
        {
            m_r2vbEntity->setVisible(isVisible);
        }
        for(auto& it : m_r2vbObject)
        {
            it.second->setVisible(m_isVisible);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SMesh::doConfigure() throw(::fwTools::Failed)
{
    std::string color = m_configuration->getAttributeValue("color");

    if(m_material)
    {
        m_material->diffuse()->setRGBA(color.empty() ? "#ffffffff" : color);
    }

    if(m_configuration->hasAttribute("autoresetcamera"))
    {
        std::string autoResetCamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoResetCamera == "yes");
    }

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created SMaterial
    if ( m_configuration->hasAttribute("materialName"))
    {
        m_materialName = m_configuration->getAttributeValue("materialName");
    }
    else
    {
        // An existing Ogre material will be used for this mesh
        if( m_configuration->hasAttribute("materialTemplate"))
        {
            m_materialTemplateName = m_configuration->getAttributeValue("materialTemplate");
        }

        // The mesh adaptor will pass the texture name to the created material adaptor
        if ( m_configuration->hasAttribute("textureName"))
        {
            m_textureName = m_configuration->getAttributeValue("textureName");
        }

        if( m_configuration->hasAttribute("shadingMode"))
        {
            m_shadingMode = m_configuration->getAttributeValue("shadingMode");
        }
    }

    if(m_configuration->hasAttribute("dynamic"))
    {
        std::string dynamic = m_configuration->getAttributeValue("dynamic");
        m_isDynamic = ( dynamic == "true" );
    }

    if(m_configuration->hasAttribute("dynamicVertices"))
    {
        std::string dynamic = m_configuration->getAttributeValue("dynamicVertices");
        m_isDynamicVertices = ( dynamic == "true" );
    }

    if(m_configuration->hasAttribute("transform"))
    {
        this->setTransformId(m_configuration->getAttributeValue("transform"));
    }
}

//-----------------------------------------------------------------------------

void SMesh::doStart() throw(::fwTools::Failed)
{
    auto& meshMgr = ::Ogre::MeshManager::getSingleton();

    // Create Mesh Data Structure
    m_meshName = this->getID() + "_Mesh";
    m_ogreMesh = meshMgr.createManual(m_meshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    // Create Mesh Data Structure for r2vb input
    // We could create it only when necessary, but for now it is simpler to create it every time
    m_r2vbMeshName = this->getID() + "_r2vbMesh";
    m_r2vbMesh     = meshMgr.createManual(m_r2vbMeshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    // We have to create a new material adaptor only if this adaptor is instanciated by a reconstruction adaptor
    // or if no material adaptor uid has been configured
    m_useNewMaterialAdaptor = m_isReconstructionManaged || m_materialName.empty();

    if(!m_useNewMaterialAdaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtlAdaptors = this->getRenderService()->getAdaptors< ::visuOgreAdaptor::SMaterial>();

        auto result =
            std::find_if(mtlAdaptors.begin(), mtlAdaptors.end(), [this](const ::visuOgreAdaptor::SMaterial::sptr& srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        m_materialAdaptor = *result;

        SLM_ASSERT("SMaterial adaptor managing material'" + m_materialName + "' is not found",
                   result != mtlAdaptors.end());
        m_material = m_materialAdaptor->getObject< ::fwData::Material >();
    }

    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();
    this->updateMesh(mesh);
}

//-----------------------------------------------------------------------------

void SMesh::doStop() throw(::fwTools::Failed)
{
    if(!m_transformService.expired())
    {
        m_transformService.lock()->stop();
        ::fwServices::OSR::unregisterService(m_transformService.lock());
    }

    this->unregisterServices();

    this->clearMesh();

    if(!m_useNewMaterialAdaptor)
    {
        m_materialAdaptor.reset();
    }

    if(m_entity)
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }
    // Destroy Ogre Mesh
    auto& meshMgr = ::Ogre::MeshManager::getSingleton();
    meshMgr.remove(m_ogreMesh->getHandle());
    meshMgr.remove(m_r2vbMesh->getHandle());
}

//-----------------------------------------------------------------------------

void SMesh::doSwap() throw(::fwTools::Failed)
{
    doStop();
    doStart();
}

//-----------------------------------------------------------------------------

void SMesh::doUpdate() throw(::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SMesh::updateMesh(const ::fwData::Mesh::sptr& mesh)
{
    ::fwDataTools::helper::Mesh meshHelper(mesh);

    /// The values in this table refer to vertices in the above table
    size_t uiNumVertices = mesh->getNumberOfPoints();
    OSLM_DEBUG("Vertices #" << uiNumVertices);

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    if(uiNumVertices <= 0)
    {
        SLM_DEBUG("Empty mesh");

        if(m_entity)
        {
            sceneMgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }
        return;
    }

    // Check if mesh attributes
    m_hasNormal = (mesh->getPointNormalsArray() != nullptr);

    this->getRenderService()->makeCurrent();

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

        if(!m_hasNormal)
        {
            // Verify if mesh contains Tetra or Edge
            // If not, generate normals
            ::fwData::Mesh::CellTypesMultiArrayType cellsType = meshHelper.getCellTypes();
            bool computeNormals = true;

            ::fwData::mt::ObjectReadLock lock(this->getObject());

            for(unsigned int i = 0; i < cellsType.size() && computeNormals; ++i)
            {
                auto cellType = cellsType[i];
                if(cellType == ::fwData::Mesh::EDGE || cellType == ::fwData::Mesh::TETRA)
                {
                    computeNormals = false;
                }
            }

            if(computeNormals)
            {
                mesh->allocatePointNormals();
                ::fwDataTools::Mesh::generatePointNormals(mesh);
                m_hasNormal = true;
            }
        }

        // We need to reallocate
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;

        // Allocate vertex buffer of the requested number of vertices (vertexCount)
        // and bytes per vertex (offset)
        ::Ogre::HardwareVertexBufferSharedPtr vbuf;
        ::Ogre::HardwareBuffer::Usage usage = (m_isDynamic || m_isDynamicVertices) ?
                                              ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE :
                                              ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

        ::Ogre::HardwareBufferManager& mgr = ::Ogre::HardwareBufferManager::getSingleton();

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
        vbuf = mgr.createVertexBuffer(offset, uiNumVertices, usage, false);
        bind.setBinding(m_binding[POSITION_NORMAL], vbuf);
    }
    else
    {
        // We don't reallocate, we keep the same vertex buffers and only update the number of vertices
        m_ogreMesh->sharedVertexData->vertexCount = uiNumVertices;
    }

    //------------------------------------------
    // Create indices arrays
    //------------------------------------------

    // 1 - Count number of primitives for each type
    ::fwData::Mesh::CellTypesMultiArrayType cellsType = meshHelper.getCellTypes();
    unsigned int numIndices[ s_numPrimitiveTypes ];

    memset(numIndices, 0, sizeof(numIndices));

    ::fwData::mt::ObjectReadLock lock(this->getObject());

    for(unsigned int i = 0; i < cellsType.size(); ++i)
    {
        auto cellType = cellsType[i];
        if(cellType == ::fwData::Mesh::TRIANGLE)
        {
            numIndices[::fwData::Mesh::TRIANGLE] += 3;
        }
        else if(cellType == ::fwData::Mesh::QUAD)
        {
            numIndices[::fwData::Mesh::QUAD] += 4;
        }
        else if(cellType == ::fwData::Mesh::EDGE)
        {
            numIndices[::fwData::Mesh::EDGE] += 2;
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

    const bool indices32Bits     = uiNumVertices >= (1 << 16);
    const bool indicesPrev32Bits = uiPrevNumVertices >= (1 << 16);
    const bool hasPrimitiveColor = (mesh->getCellColorsArray() != nullptr);

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
                    else if(cellType == ::fwData::Mesh::QUAD || cellType == ::fwData::Mesh::TETRA )
                    {
                        // Use r2vb pipeline to generate quads or tetrahedrons
                        m_subMeshes[i]                = m_r2vbMesh->createSubMesh(name);
                        m_subMeshes[i]->operationType = ::Ogre::RenderOperation::OT_LINE_LIST;
                    }

                    m_subMeshes[i]->useSharedVertices     = true;
                    m_subMeshes[i]->indexData->indexStart = 0;
                }

                ::Ogre::HardwareIndexBufferSharedPtr ibuf = m_subMeshes[i]->indexData->indexBuffer;

                // Allocate index buffer of the requested number of vertices (ibufCount) if necessary
                // We don't reallocate if we have more space than requested
                bool createIndexBuffer = ibuf.isNull();
                if( !ibuf.isNull())
                {
                    // reallocate if new mesh has more indexes or IndexType change
                    createIndexBuffer = (ibuf->getNumIndexes() < numIndices[i]) || (indicesPrev32Bits != indices32Bits);
                }
                if(createIndexBuffer)
                {
                    ::Ogre::HardwareBuffer::Usage usage = m_isDynamic ?
                                                          ::Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE :
                                                          ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

                    ibuf = ::Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                        indices32Bits ? ::Ogre::HardwareIndexBuffer::IT_32BIT : ::Ogre::HardwareIndexBuffer::IT_16BIT,
                        numIndices[i], usage);

                    m_subMeshes[i]->indexData->indexBuffer = ibuf;
                }
                m_subMeshes[i]->indexData->indexCount = numIndices[i];
                OSLM_DEBUG("Index #" << m_subMeshes[i]->indexData->indexCount );

                // Lock index data ow, we are going to write into it in the next loop
                indexBuffer[i] = ibuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
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

    if(indices32Bits)
    {
        copyIndices< std::uint32_t >( indexBuffer[::fwData::Mesh::TRIANGLE], indexBuffer[::fwData::Mesh::QUAD],
                                      indexBuffer[::fwData::Mesh::EDGE], indexBuffer[::fwData::Mesh::TETRA],
                                      meshHelper, mesh->getNumberOfCells() );
    }
    else
    {
        copyIndices< std::uint16_t >( indexBuffer[::fwData::Mesh::TRIANGLE], indexBuffer[::fwData::Mesh::QUAD],
                                      indexBuffer[::fwData::Mesh::EDGE], indexBuffer[::fwData::Mesh::TETRA],
                                      meshHelper, mesh->getNumberOfCells() );
    }

    for(size_t i = 0; i < s_numPrimitiveTypes; ++i)
    {
        if(numIndices[i] > 0)
        {
            m_subMeshes[i]->indexData->indexBuffer->unlock();
        }
    }

    lock.unlock();

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(!m_entity)
    {
        m_entity = sceneMgr->createEntity(m_ogreMesh);
        m_entity->setVisible(m_isVisible);
        sceneMgr->getRootSceneNode()->detachObject(m_entity);
    }

    //------------------------------------------
    // Update vertex attributes
    //------------------------------------------

    this->updateVertices(mesh);
    this->updateColors(mesh);
    this->updateTexCoords(mesh);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->createTransformService();
    if(m_useNewMaterialAdaptor)
    {
        this->updateNewMaterialAdaptor();
    }
    else
    {
        this->updateXMLMaterialAdaptor();
    }

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    //------------------------------------------
    // Render to vertex-buffer
    // Three cases to handle:
    // - Quads primitives generation - 2 triangles from 4 points
    // - Tetrahedrons primitives generation - 4 triangles from 4 points
    // - Per-primitive color generation - either triangles, quads or tetrahedrons
    //------------------------------------------

    if( (m_subMeshes[::fwData::Mesh::QUAD] || m_subMeshes[::fwData::Mesh::TETRA]) || hasPrimitiveColor)
    {
        if(!m_r2vbMesh->sharedVertexData)
        {
            m_r2vbMesh->sharedVertexData = new ::Ogre::VertexData();
        }

        *m_r2vbMesh->sharedVertexData->vertexBufferBinding = *m_ogreMesh->sharedVertexData->vertexBufferBinding;
        *m_r2vbMesh->sharedVertexData->vertexDeclaration   = *m_ogreMesh->sharedVertexData->vertexDeclaration;

        // Add mesh to Ogre Root Scene Node if it doesn't exist yet
        if(!m_r2vbEntity)
        {
            m_r2vbEntity = sceneMgr->createEntity(m_r2vbMesh);
            m_r2vbEntity->setVisible(m_isVisible);
            sceneMgr->getRootSceneNode()->detachObject(m_r2vbEntity);
        }

        const unsigned int numSubEntities = m_r2vbEntity->getNumSubEntities();
        for(unsigned int i = 0; i < numSubEntities; ++i)
        {
            auto subEntity = m_r2vbEntity->getSubEntity(i);
            auto subMesh   = subEntity->getSubMesh();

            const bool bQuad  = (subMesh == m_subMeshes[::fwData::Mesh::QUAD]);
            const bool bTetra = (subMesh == m_subMeshes[::fwData::Mesh::TETRA]);

            const ::fwData::Mesh::CellTypesEnum cellType = bQuad ? ::fwData::Mesh::QUAD :
                                                           bTetra ? ::fwData::Mesh::TETRA :
                                                           ::fwData::Mesh::TRIANGLE;

            if(m_r2vbMaterialAdaptor.find(cellType) == m_r2vbMaterialAdaptor.end())
            {
                // Instantiate a material adaptor for the r2vb process for this primitive type
                const std::string primName = std::to_string(cellType);
                auto r2vbMtlAdaptor        = this->createMaterialService(primName);

                r2vbMtlAdaptor->setPrimitiveType(cellType);
                r2vbMtlAdaptor->setHasPrimitiveColor(m_hasPrimitiveColor, m_perPrimitiveColorTextureName);

                r2vbMtlAdaptor->start();
                r2vbMtlAdaptor->connect();

                m_r2vbMaterialAdaptor[cellType] = r2vbMtlAdaptor;
            }
            else
            {
                // Get the material adaptor for the r2vb process for this primitive type
                auto r2vbMtlAdaptor = m_r2vbMaterialAdaptor[cellType];

                r2vbMtlAdaptor->setHasMeshNormal(m_hasNormal);
                r2vbMtlAdaptor->setHasVertexColor(m_hasVertexColor);
                r2vbMtlAdaptor->setHasPrimitiveColor(m_hasPrimitiveColor, m_perPrimitiveColorTextureName);
                r2vbMtlAdaptor->setTextureName(m_textureName);
                r2vbMtlAdaptor->setShadingMode(m_shadingMode);

                // Update the material *synchronously* otherwise the r2vb will be rendered before the shader switch
                r2vbMtlAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
            }

            if(m_r2vbObject.find(cellType) == m_r2vbObject.end())
            {
                const std::string name           = std::to_string(cellType);
                const std::string r2vbObjectName = this->getID() + "_r2vbObject" + name;
                const std::string mtlName        = m_materialAdaptor->getMaterialName();
                m_r2vbObject[cellType] = ::fwRenderOgre::R2VBRenderable::New(r2vbObjectName, subEntity, sceneMgr,
                                                                             cellType, mtlName);

                // Attach r2vb object in the scene graph
                this->attachNode(m_r2vbObject[cellType]);
                m_r2vbObject[cellType]->setVisible(m_isVisible);
            }

            m_r2vbObject[cellType]->setOutputSettings(static_cast<unsigned int>(subMesh->indexData->indexCount),
                                                      m_hasPrimitiveColor || m_hasVertexColor,
                                                      m_hasUV && m_materialAdaptor->hasDiffuseTexture(),
                                                      m_r2vbMaterialAdaptor[cellType]->getMaterialName());
        }
    }
}

//-----------------------------------------------------------------------------

void SMesh::updateVertices(const ::fwData::Mesh::sptr& mesh)
{
    FW_PROFILE_AVG("UPDATE VERTICES", 5);

    // Getting Vertex Buffer
    ::Ogre::VertexBufferBinding* bind          = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    ::Ogre::HardwareVertexBufferSharedPtr vbuf = bind->getBuffer(m_binding[POSITION_NORMAL]);

    /// Upload the index data to the card
    void* pVertex = vbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);

    // Update Ogre Mesh with ::fwData::Mesh
    ::fwData::mt::ObjectReadLock lock(mesh);
    ::fwDataTools::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();

    size_t uiStrideFloat = 3;
    if(m_hasNormal)
    {
        uiStrideFloat += 3;
    }

    typedef ::fwData::Mesh::PointValueType PointValueType;
    typedef ::fwData::Mesh::NormalValueType NormalValueType;

    // Copy position and normal of each vertices
    // Compute bounding box (for culling)
    float xMin = std::numeric_limits<float>::max();
    float yMin = std::numeric_limits<float>::max();
    float zMin = std::numeric_limits<float>::max();
    float xMax = -std::numeric_limits<float>::max();
    float yMax = -std::numeric_limits<float>::max();
    float zMax = -std::numeric_limits<float>::max();

    const unsigned int numPoints = static_cast<unsigned int>(mesh->getNumberOfPoints());

    {
        FW_PROFILE_AVG("UPDATE BBOX", 5);
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            const auto& pt0 = points[i][0];
            xMin = std::min(xMin, pt0);
            xMax = std::max(xMax, pt0);

            const auto& pt1 = points[i][1];
            yMin = std::min(yMin, pt1);
            yMax = std::max(yMax, pt1);

            const auto& pt2 = points[i][2];
            zMin = std::min(zMin, pt2);
            zMax = std::max(zMax, pt2);
        }
    }
    {
        PointValueType* __restrict pPos = static_cast< PointValueType* >( pVertex );
        FW_PROFILE_AVG("UPDATE POS", 5);
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            memcpy(pPos, &points[i][0], 3 * sizeof(PointValueType) );

            pPos += uiStrideFloat;
        }
    }

    if(m_hasNormal)
    {
        FW_PROFILE_AVG("UPDATE NORMALS", 5);
        ::fwData::Mesh::PointNormalsMultiArrayType normals = meshHelper.getPointNormals();
        NormalValueType* __restrict pNormal = static_cast< NormalValueType* >( pVertex ) + 3;

        for (unsigned int i = 0; i < numPoints; ++i)
        {
            memcpy(pNormal, &normals[i][0], 3 * sizeof(NormalValueType) );
            pNormal += uiStrideFloat;
        }
    }

    // Unlock vertex data
    vbuf->unlock();
    lock.unlock();

    m_ogreMesh->_setBounds( ::Ogre::AxisAlignedBox( xMin, yMin, zMin, xMax, yMax, zMax) );
    m_ogreMesh->_setBoundingSphereRadius( ::Ogre::Math::Sqrt( ::Ogre::Math::Sqr(xMax - xMin) +
                                                              ::Ogre::Math::Sqr(yMax - yMin) +
                                                              ::Ogre::Math::Sqr(zMax - zMin)) /2);

    if(m_materialAdaptor)
    {
        // Sends the mesh's bounding box to the material (used for normals display)
        m_materialAdaptor->setMeshBoundingBox(m_ogreMesh->getBounds());
    }
}

//-----------------------------------------------------------------------------

void SMesh::bindLayer(const ::fwData::Mesh::sptr& _mesh, BufferBinding _binding,
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

//-----------------------------------------------------------------------------

void SMesh::updateColors(const ::fwData::Mesh::sptr& mesh)
{
    FW_PROFILE_AVG("UPDATE COLORS", 5);

    ::Ogre::VertexBufferBinding* bind = m_ogreMesh->sharedVertexData->vertexBufferBinding;
    SLM_ASSERT("Invalid vertex buffer binding", bind);

    const bool hasVertexColor    = (mesh->getPointColorsArray() != nullptr);
    const bool hasPrimitiveColor = (mesh->getCellColorsArray() != nullptr);

    // 1 - Initialization
    if(hasVertexColor)
    {
        bindLayer(mesh, COLOUR, ::Ogre::VES_DIFFUSE, Ogre::VET_UBYTE4);
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

        if(m_perPrimitiveColorTexture.isNull())
        {
            m_perPrimitiveColorTextureName = this->getID() + "_PerCellColorTexture";
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
        if(!m_perPrimitiveColorTexture.isNull())
        {
            m_perPrimitiveColorTexture->freeInternalResources();
        }
        m_perPrimitiveColorTexture.setNull();
        m_perPrimitiveColorTextureName = "";
    }

    // 2 - Copy of vertices
    if (hasVertexColor)
    {
        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwDataTools::helper::Mesh meshHelper(mesh);

        // Source points
        ::Ogre::HardwareVertexBufferSharedPtr cbuf = bind->getBuffer(m_binding[COLOUR]);
        ::Ogre::RGBA* pColor                       =
            static_cast< ::Ogre::RGBA* >( cbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD) );

        // Destination
        const auto colors           = meshHelper.getPointColors();
        const std::uint8_t* icolors = reinterpret_cast<const std::uint8_t*>(colors.data());

        // Copy points
        ::fwRenderOgre::helper::Mesh::copyColors(pColor, icolors, mesh->getNumberOfPoints(), colors.shape()[1]);

        cbuf->unlock();
    }

    if(hasPrimitiveColor)
    {
        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwDataTools::helper::Mesh meshHelper(mesh);

        // Source cells
        ::Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_perPrimitiveColorTexture->getBuffer();
        pixelBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        const ::Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        ::Ogre::RGBA* pColorDest = static_cast< ::Ogre::RGBA* >( pixelBox.data );

        // Destination
        auto colors           = meshHelper.getCellColors();
        std::uint8_t* icolors = reinterpret_cast<std::uint8_t*>(colors.data());

        // Copy cells
        ::fwRenderOgre::helper::Mesh::copyColors(pColorDest, icolors, mesh->getNumberOfCells(), colors.shape()[1]);

        pixelBuffer->unlock();
    }

    if(hasVertexColor != m_hasVertexColor || hasPrimitiveColor != m_hasPrimitiveColor)
    {
        // The r2vb pipeline outputs per-vertex color if we have per-primitive color
        // Thus for the "regular" pipeline it is only viewed as per-vertex color
        if(m_materialAdaptor)
        {
            m_materialAdaptor->setHasVertexColor(hasVertexColor || hasPrimitiveColor);
            m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->asyncRun();
        }

        m_hasVertexColor    = hasVertexColor;
        m_hasPrimitiveColor = hasPrimitiveColor;
    }
}

//-----------------------------------------------------------------------------

void SMesh::updateTexCoords(const ::fwData::Mesh::sptr& mesh)
{
    m_hasUV = (mesh->getPointTexCoordsArray() != nullptr);

    // . UV Buffer - By now, we just use one UV coordinates set for each mesh
    if (m_hasUV)
    {
        bindLayer(mesh, TEXCOORD, ::Ogre::VES_TEXTURE_COORDINATES, Ogre::VET_FLOAT2);

        FW_PROFILE_AVG("UPDATE TexCoords", 5);

        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwDataTools::helper::Mesh meshHelper(mesh);

        ::Ogre::VertexBufferBinding* bind           = m_ogreMesh->sharedVertexData->vertexBufferBinding;
        ::Ogre::HardwareVertexBufferSharedPtr uvbuf = bind->getBuffer(m_binding[TEXCOORD]);
        void* pBuf = uvbuf->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        float* pUV = static_cast< float* >( pBuf );

        ::fwData::Mesh::PointTexCoordsMultiArrayType uvCoord = meshHelper.getPointTexCoords();

        // Copy UV coordinates for each mesh point
        for (unsigned int i = 0; i < mesh->getNumberOfPoints(); ++i)
        {
            pUV[2 * i]     = uvCoord[i][0];
            pUV[2 * i + 1] = uvCoord[i][1];
        }

        uvbuf->unlock();
    }
}

//-----------------------------------------------------------------------------

void SMesh::clearMesh()
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

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    for(auto r2vbObject : m_r2vbObject)
    {
        sceneMgr->destroyMovableObject(r2vbObject.second);
    }
    m_r2vbObject.clear();

    if(m_r2vbEntity)
    {
        sceneMgr->destroyEntity(m_r2vbEntity);
        m_r2vbEntity = nullptr;
    }
}

//------------------------------------------------------------------------------

::visuOgreAdaptor::SMaterial::sptr SMesh::createMaterialService(const std::string& _materialSuffix)
{
    auto matSrv = ::fwServices::add< ::fwRenderOgre::IAdaptor>(m_material, "::visuOgreAdaptor::SMaterial");
    SLM_ASSERT("Material service not instantiated", matSrv);
    auto materialAdaptor = ::visuOgreAdaptor::SMaterial::dynamicCast(matSrv);

    materialAdaptor->setID(this->getID() + "_" + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);

    if (!m_materialTemplateName.empty())
    {
        materialAdaptor->setMaterialTemplateName(m_materialTemplateName);
    }

    const std::string meshName = this->getObject()->getID();
    const std::string mtlName  = meshName + "_" + materialAdaptor->getID() + _materialSuffix;

    materialAdaptor->setMaterialName(mtlName);

    materialAdaptor->setHasMeshNormal(m_hasNormal);
    materialAdaptor->setHasVertexColor(m_hasVertexColor);
    materialAdaptor->setTextureName(m_textureName);
    materialAdaptor->setShadingMode(m_shadingMode);
    materialAdaptor->setMeshBoundingBox(m_ogreMesh->getBounds());

    this->registerService(materialAdaptor);

    return materialAdaptor;
}

//------------------------------------------------------------------------------

void SMesh::updateNewMaterialAdaptor()
{
    if(!m_materialAdaptor)
    {
        if(m_entity)
        {
            m_materialAdaptor = this->createMaterialService();
            m_materialAdaptor->start();
            m_materialAdaptor->connect();

            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
        }
    }
    else if(m_materialAdaptor->getObject< ::fwData::Material >() != m_material)
    {
        m_materialAdaptor->swap(m_material);
    }
}

//------------------------------------------------------------------------------

void SMesh::updateXMLMaterialAdaptor()
{
    if(m_materialAdaptor->getUpdatingStatus() == UpdatingStatus::NOTUPDATING)
    {
        if(m_materialAdaptor->getMaterialName().empty())
        {
            std::string meshName = this->getObject()->getID();
            m_materialAdaptor->setMaterialName(meshName + "_Material");
        }

        m_materialAdaptor->setHasMeshNormal(m_hasNormal);
        m_materialAdaptor->setHasVertexColor(m_hasVertexColor);
        m_materialAdaptor->setShadingMode(m_shadingMode);
        m_materialAdaptor->setMeshBoundingBox(m_ogreMesh->getBounds());

        if(m_entity)
        {
            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
        }
    }
    else if(m_materialAdaptor->getObject< ::fwData::Material >() != m_material)
    {
        m_materialAdaptor->swap(m_material);
    }
}

//------------------------------------------------------------------------------

void SMesh::createTransformService()
{
    // We need to create a transform service only once
    if(m_transformService.expired())
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

        // Create a transform and set it as a field of the mesh
        auto fieldTransform = ::fwData::TransformationMatrix3D::New();
        mesh->setField("TransformMatrix", fieldTransform);

        m_transformService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(fieldTransform,
                                                                           "::visuOgreAdaptor::STransform");
        SLM_ASSERT("Transform service is null", m_transformService.lock());
        auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

        transformService->setID(this->getID() + "_" + transformService->getID());
        transformService->setRenderService( this->getRenderService() );
        transformService->setLayerID(m_layerID);
        transformService->setTransformId(this->getTransformId());
        transformService->setParentTransformId(this->getParentTransformId());

        transformService->start();
        transformService->connect();
        this->registerService(transformService);

        this->attachNode(m_entity);
    }
}

//-----------------------------------------------------------------------------

void SMesh::modifyVertices()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();
    this->updateVertices(mesh);

    /// Notify -Mesh object that it has been loaded
    m_ogreMesh->load();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::modifyMesh()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();

    const bool hasVertexColor    = (mesh->getPointColorsArray() != nullptr);
    const bool hasPrimitiveColor = (mesh->getCellColorsArray() != nullptr);

    if(hasVertexColor != m_hasVertexColor || hasPrimitiveColor != m_hasPrimitiveColor)
    {
        this->clearMesh();
    }
    this->updateMesh(mesh);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::modifyPointColors()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();

    const bool hasVertexColor    = (mesh->getPointColorsArray() != nullptr);
    const bool hasPrimitiveColor = (mesh->getCellColorsArray() != nullptr);

    if(hasVertexColor != m_hasVertexColor || hasPrimitiveColor != m_hasPrimitiveColor)
    {
        this->clearMesh();
        this->updateMesh(mesh);
    }
    else
    {
        this->updateColors(mesh);
    }

    /// Notify -Mesh object that it has been loaded
    m_ogreMesh->load();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::modifyTexCoords()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();
    this->updateTexCoords(mesh);

    /// Notify -Mesh object that it has been loaded
    m_ogreMesh->load();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::attachNode(::Ogre::MovableObject* _node)
{
    auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

    ::Ogre::SceneNode* transNode = transformService->getSceneNode();
    ::Ogre::SceneNode* node      = _node->getParentSceneNode();
    if ((node != transNode) && transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SMesh::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_MODIFY_VERTICES_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG,
                                           s_MODIFY_POINT_TEX_COORDS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_MODIFIED_SIG, s_MODIFY_MESH_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

void SMesh::requestRender()
{
    for(auto r2vbObject : m_r2vbObject)
    {
        r2vbObject.second->setDirty();
    }

    ::fwRenderOgre::IAdaptor::requestRender();
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
