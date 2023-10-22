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
#include "viz/scene3d/vr/grid_proxy_geometry.hpp"

#include "viz/scene3d/factory/r2vb_renderable.hpp"
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

namespace sight::viz::scene3d::vr
{

//------------------------------------------------------------------------------

grid_proxy_geometry* grid_proxy_geometry::make(
    const std::string& _name,
    Ogre::SceneManager* _scene_manager,
    const viz::scene3d::texture::sptr& _3_d_image_texture,
    const viz::scene3d::texture::sptr& _mask_texture,
    const transfer_function::sptr& _tf,
    const std::string& _mtl_name
)
{
    SIGHT_ASSERT("scene manager cannot be nullptr.", _scene_manager != nullptr);

    auto* instance = static_cast<viz::scene3d::vr::grid_proxy_geometry*>
                     (_scene_manager->createMovableObject(_name, grid_proxy_geometry_factory::FACTORY_TYPE_NAME));

    instance->m_inputPrimitiveType = data::mesh::cell_type_t::POINT;
    instance->mParentSceneManager  = _scene_manager;
    instance->m_3DImageTexture     = _3_d_image_texture;
    instance->m_maskTexture        = _mask_texture;
    instance->m_gpuTF              = _tf;

    Ogre::MaterialPtr mat =
        Ogre::MaterialManager::getSingleton().getByName(
            std::string(_name) + "_" + _mtl_name,
            RESOURCE_GROUP
        );

    if(!mat)
    {
        mat =
            Ogre::MaterialManager::getSingleton().getByName(
                _mtl_name,
                RESOURCE_GROUP
            )->clone(std::string(_name) + "_" + _mtl_name);
    }

    instance->setMaterial(mat);

    instance->initialize();
    instance->manualUpdate();

    return instance;
}

//------------------------------------------------------------------------------

grid_proxy_geometry::grid_proxy_geometry(const Ogre::String& _name) :
    r2vb_renderable(_name)
{
}

//------------------------------------------------------------------------------

grid_proxy_geometry::~grid_proxy_geometry()
{
    this->detachFromParent(); //Safety

    if(m_r2vbSource != nullptr)
    {
        Ogre::MeshPtr mesh = m_r2vbSource->getMesh();
        m_srcObject = nullptr;

        mParentSceneManager->destroyEntity(m_r2vbSource);

        Ogre::MeshManager::getSingleton().remove(mesh->getHandle());
    }

    if(m_gridTexture != nullptr)
    {
        Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
    }

    if(m_gridViewportCamera != nullptr)
    {
        mParentSceneManager->destroyCamera(m_gridViewportCamera);
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initialize()
{
    // Create a camera to render the grid volume.
    // Using the default camera may result in strange behaviour, especially in a concurrent environment.
    // (E.g. accessing the camera's viewport size while the grid volume is rendered will return its width and height).
    if(m_gridViewportCamera == nullptr)
    {
        m_gridViewportCamera = mParentSceneManager->createCamera(mName + "_GridVolumeCamera");
    }

    this->initializeR2VBSource();
    this->initializeGridMaterials();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::updateGridSize()
{
    const std::vector<int> image_size =
    {
        static_cast<int>(m_3DImageTexture->width()),
        static_cast<int>(m_3DImageTexture->height()),
        static_cast<int>(m_3DImageTexture->depth())
    };

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        m_gridSize[i] =
            static_cast<int>(image_size[i]) / s_brickSize[i]
            + static_cast<int>(static_cast<int>(image_size[i]) % s_brickSize[i] != 0);
    }

    if(m_gridTexture)
    {
        Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
        m_gridTexture.reset();
    }

    this->setupGrid();
    this->computeGrid();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initializeR2VBSource()
{
    if(m_r2vbSource == nullptr)
    {
        //The mesh managed by m_r2vbSource
        Ogre::MeshPtr grid_mesh = Ogre::MeshManager::getSingleton().createManual(
            mName + "_gridMesh",
            viz::scene3d::RESOURCE_GROUP
        );

        //Set up its SubMesh
        {
            Ogre::SubMesh* sub_mesh = grid_mesh->createSubMesh();

            const int nb_vtx = m_gridSize[0] * m_gridSize[1] * m_gridSize[2];

            sub_mesh->useSharedVertices = false;
            sub_mesh->operationType     = Ogre::RenderOperation::OT_POINT_LIST;

            sub_mesh->vertexData              = new Ogre::VertexData();
            sub_mesh->vertexData->vertexStart = 0;
            sub_mesh->vertexData->vertexCount = static_cast<std::size_t>(nb_vtx);

            Ogre::VertexDeclaration* decl = sub_mesh->vertexData->vertexDeclaration;

            decl->addElement(0, 0, Ogre::VET_INT1, Ogre::VES_POSITION);
        }

        //Update the bounds
        {
            grid_mesh->_setBounds(Ogre::AxisAlignedBox::BOX_INFINITE);
            grid_mesh->_setBoundingSphereRadius(1000);
            grid_mesh->load();
        }

        //Create the R2VB source
        {
            m_r2vbSource = mParentSceneManager->createEntity(grid_mesh);

            m_srcObject = m_r2vbSource->getSubEntity(0);

            m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);
        }
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initializeGridMaterials()
{
    Ogre::MaterialManager& mtl_mng = Ogre::MaterialManager::getSingleton();

    if(m_gridComputingPass == nullptr)
    {
        Ogre::MaterialPtr grid_mtl = mtl_mng.getByName(this->getName() + "_VolumeBricksGrid", RESOURCE_GROUP);
        if(!grid_mtl)
        {
            grid_mtl =
                mtl_mng.getByName("VolumeBricksGrid", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricksGrid");
        }

        grid_mtl->load();
        m_gridComputingPass = grid_mtl->getTechnique(0)->getPass(0);
    }

    if(m_geomGeneratorPass == nullptr)
    {
        Ogre::MaterialPtr geom_generator_mtl = mtl_mng.getByName(this->getName() + "_VolumeBricks", RESOURCE_GROUP);
        if(!geom_generator_mtl)
        {
            geom_generator_mtl =
                mtl_mng.getByName("VolumeBricks", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricks");
        }

        geom_generator_mtl->load();
        m_geomGeneratorPass = geom_generator_mtl->getTechnique(0)->getPass(0);
    }

    this->setupGrid();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::setupGrid()
{
    // Create grid texture and initialize render targets.
    {
        m_gridTexture = Ogre::TextureManager::getSingleton().createManual(
            mName + "_gridTexture",
            viz::scene3d::RESOURCE_GROUP,
            Ogre::TEX_TYPE_3D,
            static_cast<unsigned int>(m_gridSize[0]),
            static_cast<unsigned int>(m_gridSize[1]),
            static_cast<unsigned int>(m_gridSize[2]),
            0,
            Ogre::PF_R8,
            Ogre::TU_RENDERTARGET
        );

        for(unsigned i = 0 ; i < static_cast<unsigned>(m_gridSize[2]) ; ++i)
        {
            Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);
            rt->setDepthBufferPool(Ogre::DepthBuffer::POOL_NO_DEPTH);
            rt->addViewport(m_gridViewportCamera);
        }
    }

    // Update R2VB source geometry.
    {
        Ogre::MeshPtr r2vb_src_mesh = Ogre::MeshManager::getSingleton().getByName(
            mName + "_gridMesh",
            RESOURCE_GROUP
        );

        Ogre::VertexData* mesh_vtx_data = r2vb_src_mesh->getSubMesh(0)->vertexData;

        mesh_vtx_data->vertexCount = std::size_t(m_gridSize[0]) * std::size_t(m_gridSize[1])
                                     * std::size_t(m_gridSize[2]);

        Ogre::HardwareVertexBufferSharedPtr vtx_buffer =
            Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                Ogre::VertexElement::getTypeSize(Ogre::VET_INT1),
                mesh_vtx_data->vertexCount,
                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
            );

        for(std::size_t i = 0 ; i < mesh_vtx_data->vertexCount ; ++i)
        {
            vtx_buffer->writeData(
                i * Ogre::VertexElement::getTypeSize(Ogre::VET_INT1),
                Ogre::VertexElement::getTypeSize(Ogre::VET_INT1),
                &i,
                false
            );
        }

        mesh_vtx_data->vertexBufferBinding->setBinding(0, vtx_buffer);

        r2vb_src_mesh->load();

        m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);

        // The maximum number of vertices generated by our grid to brick algorithm.
        // Worst case is when the grid has a '3D checkerboard' pattern.
        // This means half the bricks are full.
        // The transform feedback outputs triangles (not strips) meaning that
        // one brick then generates 36 vertices (6 per cube face).
        const std::size_t maximum_vertex_count = 1 + (mesh_vtx_data->vertexCount * 36 - 1) / 2; // = (vC * 36)/2 + (vC *
        // 36)%2

        this->setOutputSettings(maximum_vertex_count, false, false, false);
        this->setRenderToBufferMaterial(this->getName() + "_VolumeBricks");
    }

    // Set shader parameters.
    {
        Ogre::GpuProgramParametersSharedPtr grid_generator_params = m_gridComputingPass->getFragmentProgramParameters();

        m_gpuTF.lock()->bind(m_gridComputingPass, s_TF_TEXUNIT_NAME, grid_generator_params);
        m_3DImageTexture->bind(m_gridComputingPass, "image");

        grid_generator_params->setNamedConstant("u_brickSize", s_brickSize.data(), 3, 1);

        Ogre::GpuProgramParametersSharedPtr geom_generator_vtx_params =
            m_geomGeneratorPass->getVertexProgramParameters();
        geom_generator_vtx_params->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);

        Ogre::GpuProgramParametersSharedPtr geom_generator_geom_params =
            m_geomGeneratorPass->getGeometryProgramParameters();

        const std::vector<int> image_size = {{
            int(m_3DImageTexture->width()),
            int(m_3DImageTexture->height()),
            int(m_3DImageTexture->depth())
        }
        };

        geom_generator_geom_params->setNamedConstant("u_imageResolution", image_size.data(), 3, 1);
        geom_generator_geom_params->setNamedConstant(
            "u_brickSize",
            sight::viz::scene3d::vr::grid_proxy_geometry::s_brickSize.data(),
            3,
            1
        );

        Ogre::TextureUnitState* grid_tex_state = m_geomGeneratorPass->getTextureUnitState("gridVolume");

        SIGHT_ASSERT("'gridVolume' texture unit is not found", grid_tex_state);
        grid_tex_state->setTexture(m_gridTexture);
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::computeGrid()
{
    const std::size_t count = m_gridRenderOp.vertexData->vertexCount;
    m_gridRenderOp.vertexData->vertexCount = 4;
    m_gridRenderOp.operationType           = Ogre::RenderOperation::OT_TRIANGLE_STRIP;

    m_maskTexture->bind(m_gridComputingPass, "mask", {}, {}, false);
    Ogre::GpuProgramParametersSharedPtr params = m_gridComputingPass->getFragmentProgramParameters();
    m_gpuTF.lock()->bind(m_gridComputingPass, s_TF_TEXUNIT_NAME, params);

    for(unsigned i = 0 ; i < static_cast<unsigned>(m_gridSize[2]) ; ++i)
    {
        Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);

        params->setNamedConstant("u_slice", static_cast<int>(i));

        mParentSceneManager->manualRender(
            &m_gridRenderOp,
            m_gridComputingPass,
            rt->getViewport(0),
            Ogre::Affine3::IDENTITY,
            Ogre::Affine3::IDENTITY,
            Ogre::Matrix4::IDENTITY
        );
    }

    m_gridRenderOp.vertexData->vertexCount = count;
    m_gridRenderOp.operationType           = Ogre::RenderOperation::OT_POINT_LIST;

    this->manualUpdate();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::clipGrid(const Ogre::AxisAlignedBox& _clipping_box)
{
    Ogre::GpuProgramParametersSharedPtr geom_params = m_geomGeneratorPass->getGeometryProgramParameters();

    if(_clipping_box.isFinite())
    {
        geom_params->setNamedConstant("u_boundingBoxMin", _clipping_box.getMinimum());
        geom_params->setNamedConstant("u_boundingBoxMax", _clipping_box.getMaximum());
    }
    else if(_clipping_box.isNull())
    {
        SIGHT_ERROR("Unexpected empty clipping box, no proxy geometry will be generated.");
        geom_params->setNamedConstant("u_boundingBoxMin", Ogre::Vector3::ZERO);
        geom_params->setNamedConstant("u_boundingBoxMax", Ogre::Vector3::ZERO);
    }
    else // Infinite box
    {
        geom_params->setNamedConstant("u_boundingBoxMin", Ogre::Vector3::ZERO);
        geom_params->setNamedConstant("u_boundingBoxMax", Ogre::Vector3::UNIT_SCALE);
    }

    this->manualUpdate();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
