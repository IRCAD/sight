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

#include "viz/scene3d/vr/GridProxyGeometry.hpp"

#include "viz/scene3d/factory/R2VBRenderable.hpp"
#include "viz/scene3d/ogre.hpp"

#include <OGRE/OgreDepthBuffer.h>
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

namespace sight::viz::scene3d
{

namespace vr
{

::Ogre::String GridProxyGeometryFactory::FACTORY_TYPE_NAME = "GridProxyGeometry";

static const std::string s_TF_TEXUNIT_NAME = "transferFunction";

//------------------------------------------------------------------------------

GridProxyGeometry* GridProxyGeometry::New(
    const std::string& _name,
    ::Ogre::SceneManager* _sceneManager,
    ::Ogre::TexturePtr _3DImageTexture,
    const TransferFunction::sptr& _tf,
    const std::string& _mtlName
)
{
    const auto& factoryName = GridProxyGeometryFactory::FACTORY_TYPE_NAME;
    auto instance           = static_cast<viz::scene3d::vr::GridProxyGeometry*>
                              (_sceneManager->createMovableObject(_name, factoryName));

    instance->m_inputPrimitiveType = data::Mesh::CellType::POINT;
    instance->mParentSceneManager  = _sceneManager;
    instance->m_3DImageTexture     = _3DImageTexture;
    instance->m_gpuTF              = _tf;

    ::Ogre::MaterialPtr mat = ::Ogre::MaterialManager::getSingleton().getByName(_name + "_" + _mtlName, RESOURCE_GROUP);
    if(!mat)
    {
        mat =
            ::Ogre::MaterialManager::getSingleton().getByName(_mtlName, RESOURCE_GROUP)->clone(_name + "_" + _mtlName);
    }

    instance->setMaterial(mat);

    instance->initialize();
    instance->manualUpdate();

    return instance;
}

//------------------------------------------------------------------------------

GridProxyGeometry::GridProxyGeometry(const Ogre::String& name) :
    R2VBRenderable(name)
{
}

//------------------------------------------------------------------------------

GridProxyGeometry::~GridProxyGeometry()
{
    if(m_r2vbSource)
    {
        ::Ogre::MeshPtr mesh = m_r2vbSource->getMesh();
        this->m_srcObject = nullptr;
        mParentSceneManager->destroyEntity(m_r2vbSource);
        m_r2vbSource = nullptr;
        ::Ogre::MeshManager::getSingleton().remove(mesh->getHandle());
    }

    if(m_gridTexture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
    }

    if(m_gridViewportCamera)
    {
        mParentSceneManager->destroyCamera(m_gridViewportCamera);
    }
}

//------------------------------------------------------------------------------

void GridProxyGeometry::initialize()
{
    // Create a camera to render the grid volume.
    // Using the default camera may result in strange behaviour, especially in a concurrent environment.
    // (E.g. accessing the camera's viewport size while the grid volume is rendered will return its width and height).
    m_gridViewportCamera = mParentSceneManager->createCamera(this->mName + "_GridVolumeCamera");
    this->initializeR2VBSource();
    this->initializeGridMaterials();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::updateGridSize()
{
    const std::vector<int> imageSize = {{int(this->m_3DImageTexture->getWidth()),
        int(this->m_3DImageTexture->getHeight()),
        int(this->m_3DImageTexture->getDepth())
    }
    };

    for(size_t i = 0 ; i < 3 ; ++i)
    {
        m_gridSize[i] =
            static_cast<int>(imageSize[i]) / m_brickSize[i]
            + (static_cast<int>(imageSize[i]) % m_brickSize[i] != 0);
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
        viz::scene3d::RESOURCE_GROUP
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
    ::Ogre::MaterialManager& mtlMng = ::Ogre::MaterialManager::getSingleton();

    ::Ogre::MaterialPtr gridMtl = mtlMng.getByName(this->getName() + "_VolumeBricksGrid", RESOURCE_GROUP);
    if(!gridMtl)
    {
        gridMtl = mtlMng.getByName("VolumeBricksGrid", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricksGrid");
    }

    gridMtl->load();
    m_gridComputingPass = gridMtl->getTechnique(0)->getPass(0);

    ::Ogre::MaterialPtr geomGeneratorMtl = mtlMng.getByName(this->getName() + "_VolumeBricks", RESOURCE_GROUP);
    if(!geomGeneratorMtl)
    {
        geomGeneratorMtl = mtlMng.getByName("VolumeBricks", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricks");
    }

    geomGeneratorMtl->load();
    m_geomGeneratorPass = geomGeneratorMtl->getTechnique(0)->getPass(0);

    ::Ogre::TextureUnitState* tex3DState = m_gridComputingPass->getTextureUnitState("image");
    SIGHT_ASSERT("'image' texture unit is not found", tex3DState);
    tex3DState->setTexture(m_3DImageTexture);

    this->setupGrid();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::setupGrid()
{
    // Create grid texture and initialize render targets.
    {
        m_gridTexture = ::Ogre::TextureManager::getSingleton().createManual(
            this->mName + "_gridTexture",
            viz::scene3d::RESOURCE_GROUP,
            ::Ogre::TEX_TYPE_3D,
            static_cast<unsigned int>(m_gridSize[0]),
            static_cast<unsigned int>(m_gridSize[1]),
            static_cast<unsigned int>(m_gridSize[2]),
            0,
            ::Ogre::PF_R8,
            ::Ogre::TU_RENDERTARGET
        );

        for(unsigned i = 0 ; i < static_cast<unsigned>(m_gridSize[2]) ; ++i)
        {
            ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);
            rt->setDepthBufferPool(::Ogre::DepthBuffer::POOL_NO_DEPTH);
            rt->addViewport(m_gridViewportCamera);
        }
    }

    // Update R2VB source geometry.
    {
        ::Ogre::MeshPtr r2vbSrcMesh = ::Ogre::MeshManager::getSingleton().getByName(
            this->mName + "_gridMesh",
            RESOURCE_GROUP
        );

        ::Ogre::VertexData* meshVtxData = r2vbSrcMesh->getSubMesh(0)->vertexData;

        meshVtxData->vertexCount = static_cast<size_t>(m_gridSize[0] * m_gridSize[1] * m_gridSize[2]);

        ::Ogre::HardwareVertexBufferSharedPtr vtxBuffer =
            ::Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                meshVtxData->vertexCount,
                ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
            );

        for(size_t i = 0 ; i < meshVtxData->vertexCount ; ++i)
        {
            vtxBuffer->writeData(
                i * ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                &i,
                false
            );
        }

        meshVtxData->vertexBufferBinding->setBinding(0, vtxBuffer);

        r2vbSrcMesh->load();

        this->m_r2vbSource->getSubEntity(0)->getRenderOperation(this->m_gridRenderOp);

        // The maximum number of vertices generated by our grid to brick algorithm.
        // Worst case is when the grid has a '3D checkerboard' pattern.
        // This means half the bricks are full.
        // The transform feedback outputs triangles (not strips) meaning that
        // one brick then generates 36 vertices (6 per cube face).
        const size_t maximumVertexCount = 1 + (meshVtxData->vertexCount * 36 - 1) / 2; // = (vC * 36)/2 + (vC * 36)%2

        this->setOutputSettings(maximumVertexCount, false, false, false);
        this->setRenderToBufferMaterial(this->getName() + "_VolumeBricks");
    }

    // Set shader parameters.
    {
        ::Ogre::GpuProgramParametersSharedPtr gridGeneratorParams = m_gridComputingPass->getFragmentProgramParameters();

        m_gpuTF.lock()->bind(m_gridComputingPass, s_TF_TEXUNIT_NAME, gridGeneratorParams);

        gridGeneratorParams->setNamedConstant("u_brickSize", m_brickSize.data(), 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorVtxParams =
            m_geomGeneratorPass->getVertexProgramParameters();
        geomGeneratorVtxParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorGeomParams =
            m_geomGeneratorPass->getGeometryProgramParameters();

        const std::vector<int> imageSize = {{int(this->m_3DImageTexture->getWidth()),
            int(this->m_3DImageTexture->getHeight()),
            int(this->m_3DImageTexture->getDepth())
        }
        };

        geomGeneratorGeomParams->setNamedConstant("u_imageResolution", imageSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_brickSize", this->m_brickSize.data(), 3, 1);

        ::Ogre::TextureUnitState* gridTexState = m_geomGeneratorPass->getTextureUnitState("gridVolume");

        SIGHT_ASSERT("'gridVolume' texture unit is not found", gridTexState);
        gridTexState->setTexture(this->m_gridTexture);
    }
}

//------------------------------------------------------------------------------

void GridProxyGeometry::computeGrid()
{
    size_t count = m_gridRenderOp.vertexData->vertexCount;
    m_gridRenderOp.vertexData->vertexCount = 4;
    m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_TRIANGLE_STRIP;

    ::Ogre::GpuProgramParametersSharedPtr params = m_gridComputingPass->getFragmentProgramParameters();
    m_gpuTF.lock()->bind(m_gridComputingPass, s_TF_TEXUNIT_NAME, params);

    for(unsigned i = 0 ; i < static_cast<unsigned>(m_gridSize[2]) ; ++i)
    {
        ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);

        params->setNamedConstant("u_slice", static_cast<int>(i));

        mParentSceneManager->manualRender(
            &m_gridRenderOp,
            m_gridComputingPass,
            rt->getViewport(0),
            ::Ogre::Affine3::IDENTITY,
            ::Ogre::Affine3::IDENTITY,
            ::Ogre::Matrix4::IDENTITY
        );
    }

    m_gridRenderOp.vertexData->vertexCount = count;
    m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_POINT_LIST;

    this->manualUpdate();
}

//------------------------------------------------------------------------------

void GridProxyGeometry::clipGrid(const Ogre::AxisAlignedBox& _clippingBox)
{
    ::Ogre::GpuProgramParametersSharedPtr geomParams = m_geomGeneratorPass->getGeometryProgramParameters();

    if(_clippingBox.isFinite())
    {
        geomParams->setNamedConstant("u_boundingBoxMin", _clippingBox.getMinimum());
        geomParams->setNamedConstant("u_boundingBoxMax", _clippingBox.getMaximum());
    }
    else if(_clippingBox.isNull())
    {
        SIGHT_ERROR("Unexpected empty clipping box, no proxy geometry will be generated.");
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3::ZERO);
    }
    else // Infinite box
    {
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3::UNIT_SCALE);
    }

    this->manualUpdate();
}

//------------------------------------------------------------------------------

const Ogre::String& GridProxyGeometry::getMovableType() const
{
    return GridProxyGeometryFactory::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

void GridProxyGeometry::set3DImageTexture(const ::Ogre::TexturePtr& _texture)
{
    m_3DImageTexture = _texture;
}

//------------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
