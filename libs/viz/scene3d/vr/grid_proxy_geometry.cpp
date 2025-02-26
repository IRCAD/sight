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
    const viz::scene3d::texture::sptr& _3d_image_texture,
    const viz::scene3d::texture::sptr& _mask_texture,
    const transfer_function::sptr& _tf,
    const std::string& _mtl_name
)
{
    SIGHT_ASSERT("scene manager cannot be nullptr.", _scene_manager != nullptr);

    auto* instance = static_cast<viz::scene3d::vr::grid_proxy_geometry*>
                     (_scene_manager->createMovableObject(_name, grid_proxy_geometry_factory::FACTORY_TYPE_NAME));

    instance->m_input_primitive_type = data::mesh::cell_type_t::point;
    instance->mManager               = _scene_manager;
    instance->m_3d_image_texture     = _3d_image_texture;
    instance->m_mask_texture         = _mask_texture;
    instance->m_gpu_tf               = _tf;

    auto& mgr = Ogre::MaterialManager::getSingleton();
    auto mat  = mgr.getByName(core::id::join(_name, _mtl_name), RESOURCE_GROUP);

    if(!mat)
    {
        mat = mgr.getByName(_mtl_name, RESOURCE_GROUP)->clone(core::id::join(_name, _mtl_name));
    }

    instance->setMaterial(mat);

    instance->initialize();
    instance->manual_update();

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

    if(m_r2vb_source != nullptr)
    {
        Ogre::MeshPtr mesh = m_r2vb_source->getMesh();
        m_src_object = nullptr;

        mManager->destroyEntity(m_r2vb_source);

        Ogre::MeshManager::getSingleton().remove(mesh->getHandle());
    }

    if(m_grid_texture != nullptr)
    {
        Ogre::TextureManager::getSingleton().remove(m_grid_texture->getHandle());
    }

    if(m_grid_viewport_camera != nullptr)
    {
        mManager->destroyCamera(m_grid_viewport_camera);
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initialize()
{
    // Create a camera to render the grid volume.
    // Using the default camera may result in strange behaviour, especially in a concurrent environment.
    // (E.g. accessing the camera's viewport size while the grid volume is rendered will return its width and height).
    if(m_grid_viewport_camera == nullptr)
    {
        m_grid_viewport_camera = mManager->createCamera(mName + "_GridVolumeCamera");
    }

    this->initialize_r2vb_source();
    this->initialize_grid_materials();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::update_grid_size()
{
    const std::vector<int> image_size =
    {
        static_cast<int>(m_3d_image_texture->width()),
        static_cast<int>(m_3d_image_texture->height()),
        static_cast<int>(m_3d_image_texture->depth())
    };

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        m_grid_size[i] =
            static_cast<int>(image_size[i]) / BRICK_SIZE[i]
            + static_cast<int>(static_cast<int>(image_size[i]) % BRICK_SIZE[i] != 0);
    }

    if(m_grid_texture)
    {
        Ogre::TextureManager::getSingleton().remove(m_grid_texture->getHandle());
        m_grid_texture.reset();
    }

    this->setup_grid();
    this->compute_grid();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initialize_r2vb_source()
{
    if(m_r2vb_source == nullptr)
    {
        //The mesh managed by m_r2vbSource
        Ogre::MeshPtr grid_mesh = Ogre::MeshManager::getSingleton().createManual(
            mName + "_gridMesh",
            viz::scene3d::RESOURCE_GROUP
        );

        //Set up its SubMesh
        {
            Ogre::SubMesh* sub_mesh = grid_mesh->createSubMesh();

            const int nb_vtx = m_grid_size[0] * m_grid_size[1] * m_grid_size[2];

            sub_mesh->useSharedVertices = false;
            sub_mesh->operationType     = Ogre::RenderOperation::OT_POINT_LIST;

            sub_mesh->vertexData              = new Ogre::VertexData();
            sub_mesh->vertexData->vertexStart = 0;
            sub_mesh->vertexData->vertexCount = static_cast<Ogre::uint32>(nb_vtx);

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
            m_r2vb_source = mManager->createEntity(grid_mesh);

            m_src_object = m_r2vb_source->getSubEntity(0);

            m_r2vb_source->getSubEntity(0)->getRenderOperation(m_grid_render_op);
        }
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::initialize_grid_materials()
{
    Ogre::MaterialManager& mtl_mng = Ogre::MaterialManager::getSingleton();

    if(m_grid_computing_pass == nullptr)
    {
        Ogre::MaterialPtr grid_mtl = mtl_mng.getByName(this->getName() + "_VolumeBricksGrid", RESOURCE_GROUP);
        if(!grid_mtl)
        {
            grid_mtl =
                mtl_mng.getByName("VolumeBricksGrid", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricksGrid");
        }

        grid_mtl->load();
        m_grid_computing_pass = grid_mtl->getTechnique(0)->getPass(0);
    }

    if(m_geom_generator_pass == nullptr)
    {
        Ogre::MaterialPtr geom_generator_mtl = mtl_mng.getByName(this->getName() + "_VolumeBricks", RESOURCE_GROUP);
        if(!geom_generator_mtl)
        {
            geom_generator_mtl =
                mtl_mng.getByName("VolumeBricks", RESOURCE_GROUP)->clone(this->getName() + "_VolumeBricks");
        }

        geom_generator_mtl->load();
        m_geom_generator_pass = geom_generator_mtl->getTechnique(0)->getPass(0);
    }

    this->setup_grid();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::setup_grid()
{
    // Create grid texture and initialize render targets.
    {
        m_grid_texture = Ogre::TextureManager::getSingleton().createManual(
            mName + "_gridTexture",
            viz::scene3d::RESOURCE_GROUP,
            Ogre::TEX_TYPE_3D,
            static_cast<unsigned int>(m_grid_size[0]),
            static_cast<unsigned int>(m_grid_size[1]),
            static_cast<unsigned int>(m_grid_size[2]),
            0,
            Ogre::PF_R8,
            Ogre::TU_RENDERTARGET
        );

        for(unsigned i = 0 ; i < static_cast<unsigned>(m_grid_size[2]) ; ++i)
        {
            Ogre::RenderTexture* rt = m_grid_texture->getBuffer()->getRenderTarget(i);
            rt->setDepthBufferPool(Ogre::DepthBuffer::POOL_NO_DEPTH);
            rt->addViewport(m_grid_viewport_camera);
        }
    }

    // Update R2VB source geometry.
    {
        Ogre::MeshPtr r2vb_src_mesh = Ogre::MeshManager::getSingleton().getByName(
            mName + "_gridMesh",
            RESOURCE_GROUP
        );

        Ogre::VertexData* mesh_vtx_data = r2vb_src_mesh->getSubMesh(0)->vertexData;

        mesh_vtx_data->vertexCount = Ogre::uint32(m_grid_size[0] * m_grid_size[1] * m_grid_size[2]);

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

        m_r2vb_source->getSubEntity(0)->getRenderOperation(m_grid_render_op);

        // The maximum number of vertices generated by our grid to brick algorithm.
        // Worst case is when the grid has a '3D checkerboard' pattern.
        // This means half the bricks are full.
        // The transform feedback outputs triangles (not strips) meaning that
        // one brick then generates 36 vertices (6 per cube face).
        const std::size_t maximum_vertex_count = 1 + (mesh_vtx_data->vertexCount * 36 - 1) / 2; // = (vC * 36)/2 + (vC *
        // 36)%2

        this->set_output_settings(maximum_vertex_count, false, false, false);
        this->set_render_to_buffer_material(this->getName() + "_VolumeBricks");
    }

    // Set shader parameters.
    {
        Ogre::GpuProgramParametersSharedPtr grid_generator_params =
            m_grid_computing_pass->getFragmentProgramParameters();

        m_gpu_tf.lock()->bind(m_grid_computing_pass, TF_TEXUNIT_NAME, grid_generator_params);
        m_3d_image_texture->bind(m_grid_computing_pass, "image");

        grid_generator_params->setNamedConstant("u_brickSize", BRICK_SIZE.data(), 3, 1);

        Ogre::GpuProgramParametersSharedPtr geom_generator_vtx_params =
            m_geom_generator_pass->getVertexProgramParameters();
        geom_generator_vtx_params->setNamedConstant("u_gridResolution", m_grid_size.data(), 3, 1);

        Ogre::GpuProgramParametersSharedPtr geom_generator_geom_params =
            m_geom_generator_pass->getGeometryProgramParameters();

        const std::vector<int> image_size = {{
            int(m_3d_image_texture->width()),
            int(m_3d_image_texture->height()),
            int(m_3d_image_texture->depth())
        }
        };

        geom_generator_geom_params->setNamedConstant("u_imageResolution", image_size.data(), 3, 1);
        geom_generator_geom_params->setNamedConstant(
            "u_brickSize",
            sight::viz::scene3d::vr::grid_proxy_geometry::BRICK_SIZE.data(),
            3,
            1
        );

        Ogre::TextureUnitState* grid_tex_state = m_geom_generator_pass->getTextureUnitState("gridVolume");

        SIGHT_ASSERT("'gridVolume' texture unit is not found", grid_tex_state);
        grid_tex_state->setTexture(m_grid_texture);
    }
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::compute_grid()
{
    const std::size_t count = m_grid_render_op.vertexData->vertexCount;
    m_grid_render_op.vertexData->vertexCount = 4;
    m_grid_render_op.operationType           = Ogre::RenderOperation::OT_TRIANGLE_STRIP;

    m_mask_texture->bind(m_grid_computing_pass, "mask", {}, {}, false);
    Ogre::GpuProgramParametersSharedPtr params = m_grid_computing_pass->getFragmentProgramParameters();
    m_gpu_tf.lock()->bind(m_grid_computing_pass, TF_TEXUNIT_NAME, params);

    for(unsigned i = 0 ; i < static_cast<unsigned>(m_grid_size[2]) ; ++i)
    {
        Ogre::RenderTexture* rt = m_grid_texture->getBuffer()->getRenderTarget(i);

        params->setNamedConstant("u_slice", static_cast<int>(i));

        mManager->manualRender(
            &m_grid_render_op,
            m_grid_computing_pass,
            rt->getViewport(0),
            Ogre::Affine3::IDENTITY,
            Ogre::Affine3::IDENTITY,
            Ogre::Matrix4::IDENTITY
        );
    }

    m_grid_render_op.vertexData->vertexCount = Ogre::uint32(count);
    m_grid_render_op.operationType           = Ogre::RenderOperation::OT_POINT_LIST;

    this->manual_update();
}

//------------------------------------------------------------------------------

void grid_proxy_geometry::clip_grid(const Ogre::AxisAlignedBox& _clipping_box)
{
    Ogre::GpuProgramParametersSharedPtr geom_params = m_geom_generator_pass->getGeometryProgramParameters();

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

    this->manual_update();
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
