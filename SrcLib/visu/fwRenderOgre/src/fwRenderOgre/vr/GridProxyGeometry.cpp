/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/GridProxyGeometry.hpp"

#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/Layer.hpp"

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <vector>

namespace fwRenderOgre
{
namespace vr
{

::Ogre::String GridProxyGeometryFactory::FACTORY_TYPE_NAME = "GridProxyGeometry";

//------------------------------------------------------------------------------

GridProxyGeometry* GridProxyGeometry::New(const std::string& _name, ::Ogre::SceneManager* _sceneManager,
                                          ::Ogre::TexturePtr _3DImageTexture, TransferFunction& _tf,
                                          const std::string& _mtlName)
{

    const auto& factoryName = GridProxyGeometryFactory::FACTORY_TYPE_NAME;
    auto instance           = static_cast< ::fwRenderOgre::vr::GridProxyGeometry*>
                              (_sceneManager->createMovableObject(_name, factoryName));

    instance->m_inputPrimitiveType = ::fwData::Mesh::POINT;
    instance->mParentSceneManager  = _sceneManager;
    instance->m_3DImageTexture     = _3DImageTexture;
    instance->m_gpuTF              = _tf;

    ::Ogre::MaterialPtr mat = ::Ogre::MaterialManager::getSingleton().getByName(_mtlName);
    instance->setMaterial(mat);

    instance->initialize();
    instance->manualUpdate();

    return instance;
}

//------------------------------------------------------------------------------

GridProxyGeometry::GridProxyGeometry(const Ogre::String& name) :
    R2VBRenderable(name),
    m_r2vbSource(nullptr)
{
    // Initialize them here otherwise sheldon goes bananas.
    m_gridSize  = {{ 2, 2, 2 }};
    m_brickSize = {{ 8, 8, 8 }};
}

//------------------------------------------------------------------------------

GridProxyGeometry::~GridProxyGeometry()
{
    if(m_r2vbSource)
    {
        ::Ogre::MeshPtr mesh = m_r2vbSource->getMesh();
        this->m_srcObject    = nullptr;
        mParentSceneManager->destroyEntity(m_r2vbSource);
        m_r2vbSource = nullptr;
        ::Ogre::MeshManager::getSingleton().remove(mesh->getHandle());
    }

    if(m_gridTexture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
    }
}

//------------------------------------------------------------------------------

void GridProxyGeometry::initialize()
{
    this->initializeR2VBSource();
    this->initializeGridMaterials();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::updateGridSize()
{
    const std::vector<int> imageSize = {{ int(this->m_3DImageTexture->getWidth()),
                                          int(this->m_3DImageTexture->getHeight()),
                                          int(this->m_3DImageTexture->getDepth()) }};

    for(size_t i = 0; i < 3; ++i)
    {
        m_gridSize[i] =
            static_cast<int>(imageSize[i]) / m_brickSize[i] +
            (static_cast<int>(imageSize[i]) % m_brickSize[i] != 0);
    }

    if(m_gridTexture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
        m_gridTexture.reset();
    }

    this->setupGrid();
    this->computeGrid();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::initializeR2VBSource()
{
    ::Ogre::MeshPtr gridMesh = ::Ogre::MeshManager::getSingleton().createManual(
        this->mName + "_gridMesh",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
        );

    ::Ogre::SubMesh* subMesh = gridMesh->createSubMesh();

    const int nbVtx = m_gridSize[0] * m_gridSize[1] * m_gridSize[2];

    subMesh->useSharedVertices = false;
    subMesh->operationType     = ::Ogre::RenderOperation::OT_POINT_LIST;

    subMesh->vertexData              = new ::Ogre::VertexData();
    subMesh->vertexData->vertexStart = 0;
    subMesh->vertexData->vertexCount = static_cast<size_t>(nbVtx);

    ::Ogre::VertexDeclaration* decl = subMesh->vertexData->vertexDeclaration;

    decl->addElement(0, 0, ::Ogre::VET_INT1, ::Ogre::VES_POSITION);

    gridMesh->_setBounds(::Ogre::AxisAlignedBox::BOX_INFINITE);
    gridMesh->_setBoundingSphereRadius(1000);
    gridMesh->load();

    this->m_r2vbSource = this->mParentSceneManager->createEntity(gridMesh);

    while(!this->m_r2vbSource->isInitialised())
    {
        this->m_r2vbSource->_initialise();
    }

    this->m_srcObject = m_r2vbSource->getSubEntity(0);

    m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);
}

//------------------------------------------------------------------------------

void GridProxyGeometry::initializeGridMaterials()
{
    ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

    ::Ogre::Pass* gridPass = gridMtl->getTechnique(0)->getPass(0);

    ::Ogre::TextureUnitState* tex3DState = gridPass->getTextureUnitState("image");
    ::Ogre::TextureUnitState* texTFState = gridPass->getTextureUnitState("transferFunction");

    SLM_ASSERT("'image' texture unit is not found", tex3DState);
    SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

    tex3DState->setTexture(m_3DImageTexture);
    texTFState->setTexture(m_gpuTF.getTexture());

    this->setupGrid();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::setupGrid()
{
    // Create grid texture and initialize render targets.
    {
        m_gridTexture = ::Ogre::TextureManager::getSingleton().createManual(
            this->mName + "_gridTexture",
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            ::Ogre::TEX_TYPE_3D,
            static_cast<unsigned int>(m_gridSize[0]),
            static_cast<unsigned int>(m_gridSize[1]),
            static_cast<unsigned int>(m_gridSize[2]),
            1,
            ::Ogre::PF_R8,
            ::Ogre::TU_RENDERTARGET
            );

        for(unsigned i = 0; i < static_cast<unsigned>(m_gridSize[2]); ++i)
        {
            ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);
            rt->addViewport(mParentSceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME));
        }
    }

    // Update R2VB source geometry.
    {
        ::Ogre::MeshPtr r2vbSrcMesh = ::Ogre::MeshManager::getSingleton().getByName(this->mName + "_gridMesh");

        ::Ogre::VertexData* meshVtxData = r2vbSrcMesh->getSubMesh(0)->vertexData;

        meshVtxData->vertexCount = static_cast<size_t>(m_gridSize[0] * m_gridSize[1] * m_gridSize[2]);

        ::Ogre::HardwareVertexBufferSharedPtr vtxBuffer =
            ::Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                meshVtxData->vertexCount,
                ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        for(size_t i = 0; i < meshVtxData->vertexCount; ++i)
        {
            vtxBuffer->writeData(
                i * ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                &i,
                false);
        }

        meshVtxData->vertexBufferBinding->setBinding(0, vtxBuffer);

        r2vbSrcMesh->load();

        this->m_r2vbSource->getSubEntity(0)->getRenderOperation(this->m_gridRenderOp);

        this->setOutputSettings(meshVtxData->vertexCount * 36, false, false);
        this->setRenderToBufferMaterial("VolumeBricks");
    }

    // Set shader parameters.
    {
        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

        ::Ogre::Pass* gridPass = gridMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr gridGeneratorParams = gridPass->getFragmentProgramParameters();

        gridGeneratorParams->setNamedConstant("u_brickSize", m_brickSize.data(), 3, 1);

        ::Ogre::MaterialPtr geomGeneratorMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricks");
        ::Ogre::Pass* geomGenerationPass     = geomGeneratorMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorVtxParams = geomGenerationPass->getVertexProgramParameters();
        geomGeneratorVtxParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorGeomParams =
            geomGenerationPass->getGeometryProgramParameters();

        const std::vector<int> imageSize = {{ int(this->m_3DImageTexture->getWidth()),
                                              int(this->m_3DImageTexture->getHeight()),
                                              int(this->m_3DImageTexture->getDepth()) }};

        geomGeneratorGeomParams->setNamedConstant("u_imageResolution", imageSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_brickSize", this->m_brickSize.data(), 3, 1);

        ::Ogre::TextureUnitState* gridTexState = geomGenerationPass->getTextureUnitState("gridVolume");

        SLM_ASSERT("'gridVolume' texture unit is not found", gridTexState);
        gridTexState->setTexture(this->m_gridTexture);
    }
}

//------------------------------------------------------------------------------

void GridProxyGeometry::computeGrid()
{
    size_t count = m_gridRenderOp.vertexData->vertexCount;
    m_gridRenderOp.vertexData->vertexCount = 4;
    m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_TRIANGLE_STRIP;

    for(unsigned i = 0; i < static_cast<unsigned>(m_gridSize[2]); ++i)
    {
        ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);

        rt->getViewport(0)->clear();

        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

        ::Ogre::Pass* gridPass                       = gridMtl->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr params = gridPass->getFragmentProgramParameters();

        params->setNamedConstant("u_slice", static_cast<int>(i));

        mParentSceneManager->manualRender(
            &m_gridRenderOp,
            gridPass,
            rt->getViewport(0),
            ::Ogre::Matrix4::IDENTITY,
            ::Ogre::Matrix4::IDENTITY,
            ::Ogre::Matrix4::IDENTITY);
    }

    m_gridRenderOp.vertexData->vertexCount = count;
    m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_POINT_LIST;

    this->manualUpdate();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::clipGrid(const Ogre::AxisAlignedBox& _clippingBox)
{
    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox realClippingBox = maxBoxSize.intersection(_clippingBox);

    ::Ogre::MaterialPtr geomMtl =
        ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricks");
    ::Ogre::GpuProgramParametersSharedPtr geomParams =
        geomMtl->getTechnique(0)->getPass(0)->getGeometryProgramParameters();

    if(realClippingBox.isFinite())
    {
        geomParams->setNamedConstant("u_boundingBoxMin", realClippingBox.getMinimum());
        geomParams->setNamedConstant("u_boundingBoxMax", realClippingBox.getMaximum());
    }
    else if(realClippingBox.isNull())
    {
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3(std::nanf("")));
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(std::nanf("")));
    }
    else // Infinite box
    {
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(1.f, 1.f, 1.f));
    }

    this->manualUpdate();
}

//------------------------------------------------------------------------------

const Ogre::String& GridProxyGeometry::getMovableType() const
{
    return GridProxyGeometryFactory::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

} // namespace vr
} // namespace fwRenderOgre

