/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SVideo.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCommon.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_TF_SLOT     = "updateTF";
static const core::com::slots::key_t UPDATE_PL_SLOT     = "updatePL";
static const core::com::slots::key_t SET_FILTERING_SLOT = "setFiltering";
static const core::com::slots::key_t SCALE_SLOT         = "scale";

static const std::string s_VIDEO_MATERIAL_NAME             = "Video";
static const std::string s_VIDEO_WITH_TF_MATERIAL_NAME     = "VideoWithTF";
static const std::string s_VIDEO_WITH_TF_INT_MATERIAL_NAME = "VideoWithTF_Int";

//------------------------------------------------------------------------------

SVideo::SVideo() noexcept
{
    new_slot(UPDATE_TF_SLOT, &SVideo::updateTF, this);
    new_slot(UPDATE_PL_SLOT, &SVideo::updatePL, this);
    new_slot(SET_FILTERING_SLOT, &SVideo::setFiltering, this);
    new_slot(SCALE_SLOT, &SVideo::scale, this);
}

//------------------------------------------------------------------------------

void SVideo::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    const bool visible = config.get<bool>(config::s_VISIBLE, m_isVisible);
    this->updateVisibility(visible);

    m_materialTemplateName = config.get<std::string>(config::s_MATERIAL_TEMPLATE, m_materialTemplateName);
    m_textureName          = config.get<std::string>(config::s_TEXTURE_NAME, m_textureName);
    m_filtering            = config.get<bool>(config::s_FILTERING, m_filtering);
    m_scaling              = config.get<bool>(config::s_SCALING, m_scaling);
    m_radius               = config.get<std::string>(config::s_RADIUS, m_radius);
    m_displayLabel         = config.get<std::string>(config::s_DISPLAY_LABEL, m_displayLabel);
    m_labelColor           = config.get<std::string>(config::s_LABEL_COLOR, m_labelColor);
    m_color                = config.get<std::string>(config::s_COLOR, m_color);
    m_fixedSize            = config.get<std::string>(config::s_FIXED_SIZE, m_fixedSize);
    m_queryFlags           = config.get<std::string>(config::s_QUERY, m_queryFlags);
    m_fontSource           = config.get<std::string>(config::s_FONT_SOURCE, m_fontSource);
    m_fontSize             = config.get<std::string>(config::s_FONT_SIZE, m_fontSize);
}

//------------------------------------------------------------------------------

void SVideo::starting()
{
    this->initialize();

    const auto plW = m_pl.lock();

    if(plW)
    {
        m_pointList = std::make_shared<data::PointList>();

        updatePL();

        m_pointListAdaptor = this->registerService<sight::viz::scene3d::adaptor>(
            "sight::module::viz::scene3d::adaptor::SPointList"
        );

        m_pointListAdaptor->setInput(m_pointList, s_PL_INPUT, true);

        service::config_t config;
        config.add(s_CONFIG + "layer", this->getLayerID());
        config.add(s_CONFIG + "autoresetcamera", "false");
        if(!m_materialTemplateName.empty())
        {
            config.add(config::s_MATERIAL_TEMPLATE, m_materialTemplateName);
        }

        if(!m_textureName.empty())
        {
            config.add(config::s_TEXTURE_NAME, m_textureName);
        }

        if(!m_radius.empty())
        {
            config.add(config::s_RADIUS, m_radius);
        }

        if(!m_displayLabel.empty())
        {
            config.add(config::s_DISPLAY_LABEL, m_displayLabel);
        }

        if(!m_labelColor.empty())
        {
            config.add(config::s_LABEL_COLOR, m_labelColor);
        }

        if(!m_color.empty())
        {
            config.add(config::s_COLOR, m_color);
        }

        if(!m_fixedSize.empty())
        {
            config.add(config::s_FIXED_SIZE, m_fixedSize);
        }

        if(!m_queryFlags.empty())
        {
            config.add(config::s_QUERY, m_queryFlags);
        }

        if(!m_fontSource.empty())
        {
            config.add(config::s_FONT_SOURCE, m_fontSource);
        }

        if(!m_fontSize.empty())
        {
            config.add(config::s_FONT_SIZE, m_fontSize);
        }

        m_pointListAdaptor->setConfiguration(config);

        m_pointListAdaptor->setRenderService(this->getRenderService());

        m_pointListAdaptor->configure();
        m_pointListAdaptor->start();

        SIGHT_ASSERT("SPointList is not started", m_pointListAdaptor->isStarted());
    }

    {
        const auto image = m_image.lock();
        m_texture = std::make_shared<sight::viz::scene3d::Texture>(image.get_shared());
    }
}

//-----------------------------------------------------------------------------

service::connections_t SVideo::getAutoConnections() const
{
    service::connections_t connections;
    connections.push(s_IMAGE_INPUT, data::Image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INPUT, data::Image::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(s_TF_INPUT, data::TransferFunction::MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(s_TF_INPUT, data::TransferFunction::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(s_TF_INPUT, data::TransferFunction::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT);

    connections.push(s_PL_INPUT, data::PointList::MODIFIED_SIG, UPDATE_PL_SLOT);
    connections.push(s_PL_INPUT, data::PointList::POINT_ADDED_SIG, UPDATE_PL_SLOT);
    connections.push(s_PL_INPUT, data::PointList::POINT_REMOVED_SIG, UPDATE_PL_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SVideo::updating()
{
    this->getRenderService()->makeCurrent();

    const auto&& typeAndSize = [this]
                               {
                                   const auto image = m_image.lock();
                                   return std::make_pair(image->getType(), image->size());
                               }();
    const auto type = typeAndSize.first;
    const auto size = typeAndSize.second;
    if(size[0] == 0 || size[1] == 0)
    {
        return;
    }

    if(!m_isTextureInit || type != m_previousType)
    {
        auto& mtlMgr  = Ogre::MaterialManager::getSingleton();
        const auto tf = m_tf.lock();

        Ogre::MaterialPtr defaultMat;
        if(tf)
        {
            if(type == core::type::FLOAT || type == core::type::DOUBLE)
            {
                defaultMat = mtlMgr.getByName(s_VIDEO_WITH_TF_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
            }
            else
            {
                defaultMat = mtlMgr.getByName(s_VIDEO_WITH_TF_INT_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
            }
        }
        else
        {
            defaultMat = mtlMgr.getByName(s_VIDEO_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
        }

        // Duplicate the template material to create our own material
        auto material = Ogre::MaterialManager::getSingletonPtr()->createOrRetrieve(
            this->get_id() + "_VideoMaterial",
            sight::viz::scene3d::RESOURCE_GROUP,
            true
        ).first;
        m_material = Ogre::dynamic_pointer_cast<Ogre::Material>(material);

        defaultMat->copyDetailsTo(m_material);

        // Set the texture to the main material pass
        this->updateTextureFiltering();

        if(tf)
        {
            // TF texture initialization
            m_gpuTF = std::make_unique<sight::viz::scene3d::TransferFunction>(tf.get_shared());
            this->updateTF();
        }

        m_previousType = type;
    }

    m_texture->update();

    const auto layer                     = this->getLayer();
    const Ogre::Viewport* const viewport = layer->getViewport();
    SIGHT_ASSERT("The current viewport cannot be retrieved.", viewport);

    if(!m_isTextureInit || size[0] != m_previousWidth || size[1] != m_previousHeight
       // If scaling is disabled and one of the viewport coordinate is modified
       // Then we need to trigger an update of the viewport displaying the texture
       || (!m_scaling
           && (viewport->getActualWidth() != m_previousViewportWidth
               || viewport->getActualHeight() != m_previousViewportHeight))
       || m_forcePlaneUpdate)
    {
        Ogre::Pass* pass = m_material->getTechnique(0)->getPass(0);
        m_texture->bind(pass, "image");

        this->clearEntity();

        // /////////////////////////////////////////////////////////////////////
        // Create the plane entity
        // /////////////////////////////////////////////////////////////////////
        const std::string thisID        = this->get_id();
        const std::string videoMeshName = thisID + "_VideoMesh";
        const std::string entityName    = thisID + "_VideoEntity";
        const std::string nodeName      = thisID + "_VideoSceneNode";

        Ogre::MovablePlane plane(Ogre::Vector3::UNIT_Z, 0);

        Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();

        m_mesh = meshManager.createPlane(
            videoMeshName,
            sight::viz::scene3d::RESOURCE_GROUP,
            plane,
            static_cast<Ogre::Real>(size[0]),
            static_cast<Ogre::Real>(size[1])
        );

        Ogre::SceneManager* sceneManager = this->getSceneManager();
        SIGHT_ASSERT("The current scene manager cannot be retrieved.", sceneManager);

        // Create Ogre Entity
        m_entity = sceneManager->createEntity(entityName, videoMeshName);
        m_entity->setMaterial(m_material);

        // Add the entity to the scene
        m_sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
        m_sceneNode->attachObject(m_entity);

        // Slightly offset the plane in Z to allow some space for other entities, thus they can be rendered on top
        m_sceneNode->setPosition(0, 0, -1);
        m_sceneNode->setVisible(m_isVisible);

        Ogre::Camera* cam = layer->getDefaultCamera();
        SIGHT_ASSERT("Default camera not found", cam);
        cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

        if(m_scaling)
        {
            cam->setOrthoWindowHeight(static_cast<Ogre::Real>(size[1]));
        }
        else
        {
            cam->setOrthoWindowHeight(static_cast<Ogre::Real>(viewport->getActualHeight()));
            cam->setOrthoWindowWidth(static_cast<Ogre::Real>(viewport->getActualWidth()));
        }

        // Make sure no further scaling request is required
        m_forcePlaneUpdate = false;

        m_isTextureInit = true;
    }

    m_previousWidth  = size[0];
    m_previousHeight = size[1];

    m_previousViewportWidth  = viewport->getActualWidth();
    m_previousViewportHeight = viewport->getActualHeight();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::stopping()
{
    this->getRenderService()->makeCurrent();

    if(m_pointListAdaptor)
    {
        this->unregisterService(m_pointListAdaptor);
        m_pointList.reset();
    }

    this->clearEntity();

    m_texture.reset();
    m_material.reset();
    m_gpuTF.reset();

    m_isTextureInit = false;
}

//-----------------------------------------------------------------------------

void SVideo::setVisible(bool _visible)
{
    if(m_entity != nullptr)
    {
        m_entity->setVisible(_visible);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SVideo::updateTF()
{
    if(m_gpuTF)
    {
        m_gpuTF->update();

        Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
        m_gpuTF->bind(ogrePass, "tf", ogrePass->getFragmentProgramParameters());

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SVideo::updatePL()
{
    const auto image = m_image.lock();
    const auto pl    = m_pl.lock();

    const data::PointList::PointListContainer& inPoints = pl->getPoints();

    data::PointList::PointListContainer& outPoints = m_pointList->getPoints();
    outPoints.clear();

    for(const auto& inPoint : inPoints)
    {
        const data::Point::PointCoordArrayType& point = inPoint->getCoord();
        outPoints.push_back(
            std::make_shared<data::Point>(
                point[0] - static_cast<double>(image->size()[0]) * 0.5,
                -(point[1] - static_cast<double>(image->size()[1]) * 0.5),
                point[2]
            )
        );
    }

    // Send the signal:
    auto modifiedSig = m_pointList->signal<data::PointList::ModifiedSignalType>(
        data::PointList::MODIFIED_SIG
    );
    modifiedSig->async_emit();
}

//------------------------------------------------------------------------------

void SVideo::clearEntity()
{
    if(m_entity != nullptr)
    {
        m_entity->detachFromParent();
        this->getSceneManager()->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    if(m_sceneNode != nullptr)
    {
        m_sceneNode->removeAndDestroyAllChildren();
        this->getSceneManager()->destroySceneNode(m_sceneNode);
        m_sceneNode = nullptr;
    }

    if(m_mesh)
    {
        Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
        meshManager.remove(m_mesh->getName(), sight::viz::scene3d::RESOURCE_GROUP);
        m_mesh.reset();
    }
}

//------------------------------------------------------------------------------

void SVideo::setFiltering(bool filtering)
{
    m_filtering = filtering;

    // Only allow updating when the texture has been initialized
    // Otherwise, we might end up in cases where the slot is called before the
    // initialization, thus crashing the app
    if(m_isTextureInit)
    {
        this->updateTextureFiltering();
    }
}

//------------------------------------------------------------------------------

void SVideo::updateTextureFiltering()
{
    Ogre::Pass* pass = m_material->getTechnique(0)->getPass(0);
    SIGHT_ASSERT("The current pass cannot be retrieved.", pass);
    Ogre::TextureUnitState* tus = pass->getTextureUnitState("image");
    SIGHT_ASSERT("The texture unit cannot be retrieved.", tus);

    // Set up texture filtering
    tus->setTextureFiltering(m_filtering ? Ogre::TFO_BILINEAR : Ogre::TFO_NONE);
}

//------------------------------------------------------------------------------

void SVideo::scale(bool value)
{
    m_scaling          = value;
    m_forcePlaneUpdate = true;

    // Only allow updating when the texture has been initialized
    // Otherwise, we might end up in cases where the slot is called before the
    // initialization, thus crashing the app
    if(m_isTextureInit)
    {
        this->updating();
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
