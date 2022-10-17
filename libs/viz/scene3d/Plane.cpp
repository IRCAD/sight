/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "viz/scene3d/Plane.hpp"

#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

Plane::Plane(
    const core::tools::fwID::IDType& _negatoId,
    Ogre::SceneNode* _parentSceneNode,
    Ogre::SceneManager* _sceneManager,
    viz::scene3d::Texture::sptr _tex,
    filter_t _filtering,
    bool _displayBorder,
    float _entityOpacity
) :
    m_filtering(_filtering),
    m_texture(std::move(_tex)),
    m_sceneManager(_sceneManager),
    m_parentSceneNode(_parentSceneNode),
    m_displayBorder(_displayBorder),
    m_entityOpacity(_entityOpacity)
{
    // We need an internal counter to avoid naming conflicts
    static std::uint32_t count = 0;
    m_slicePlaneName = _negatoId + "_Mesh" + std::to_string(count);
    m_entityName     = _negatoId + "_Entity" + std::to_string(count);
    m_sceneNodeName  = _negatoId + "_SceneNode" + std::to_string(count++);

    // Creates the parent's child scene node positionned at (0; 0; 0)
    m_planeSceneNode = m_parentSceneNode->createChildSceneNode(m_sceneNodeName);

    auto& materialMgr            = Ogre::MaterialManager::getSingleton();
    Ogre::MaterialPtr defaultMat = materialMgr.getByName("Negato", RESOURCE_GROUP);
    SIGHT_ASSERT("Default material not found, the 'material' module may not be loaded.", defaultMat);

    // If the texture material exists, delete it.
    if(m_texMaterial)
    {
        materialMgr.remove(m_texMaterial);
        m_texMaterial.reset();
    }

    m_texMaterial = defaultMat->clone(m_slicePlaneName + "_TextMaterial");

    const Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    if(m_displayBorder)
    {
        // If the border material exist, delete it.
        if(m_borderMaterial)
        {
            materialMgr.remove(m_borderMaterial);
            m_borderMaterial.reset();
        }

        m_borderMaterial = Ogre::MaterialManager::getSingleton().getByName("BasicAmbient", RESOURCE_GROUP);
        m_borderMaterial = m_borderMaterial->clone(m_slicePlaneName + "_BorderMaterial");
    }
}

//-----------------------------------------------------------------------------

Plane::~Plane()
{
    m_planeSceneNode->removeAndDestroyAllChildren();
    m_parentSceneNode->removeAndDestroyChild(m_planeSceneNode);

    if(m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->destroyEntity(m_entityName);
    }

    if(m_slicePlane)
    {
        Ogre::MeshManager::getSingleton().remove(m_slicePlane);
    }

    if(m_border != nullptr)
    {
        m_sceneManager->destroyManualObject(m_border);
    }

    if(m_texMaterial)
    {
        Ogre::MaterialManager::getSingleton().remove(m_texMaterial);
    }

    if(m_borderMaterial)
    {
        Ogre::MaterialManager::getSingleton().remove(m_borderMaterial);
    }
}

//-----------------------------------------------------------------------------

void Plane::update(
    OrientationMode _orientation,
    const Ogre::Vector3& _spacing,
    const Ogre::Vector3& _origin,
    bool _enableTransparency
)
{
    m_orientation = _orientation;
    m_origin      = _origin;

    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();

    // First delete mesh if it already exists.
    if(meshManager.resourceExists(m_slicePlaneName, RESOURCE_GROUP))
    {
        meshManager.remove(m_slicePlaneName, RESOURCE_GROUP);
        m_slicePlane.reset();
    }

    if(m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->getEntity(m_entityName)->detachFromParent();
        m_sceneManager->destroyEntity(m_entityName);
    }

    Ogre::MovablePlane plane = this->setDimensions(_spacing);

    // Mesh plane instantiation:
    // Y is the default upVector,
    // so if we want a plane which normal is the Y unit vector we have to create it differently.
    if(m_orientation == OrientationMode::Y_AXIS)
    {
        m_slicePlane = meshManager.createPlane(
            m_slicePlaneName,
            viz::scene3d::RESOURCE_GROUP,
            plane,
            m_width,
            m_height,
            1,
            1,
            true,
            1,
            1.0,
            1.0,
            Ogre::Vector3::UNIT_Z
        );
    }
    else
    {
        m_slicePlane = meshManager.createPlane(
            m_slicePlaneName,
            viz::scene3d::RESOURCE_GROUP,
            plane,
            m_width,
            m_height
        );
    }

    // Entity creation.
    Ogre::Entity* planeEntity = m_sceneManager->createEntity(m_entityName, m_slicePlane);
    planeEntity->setMaterial(m_texMaterial);
    m_planeSceneNode->attachObject(planeEntity);

    const int orientationIndex = static_cast<int>(m_orientation);

    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto* const tech : techniques)
    {
        SIGHT_ASSERT("Technique is not set", tech);

        if(viz::scene3d::helper::Shading::isColorTechnique(*tech))
        {
            // Sets the texture filtering in the current texture unit state according to the negato's interpolation flag
            Ogre::TextureFilterOptions filterType = Ogre::TFO_NONE;
            switch(m_filtering)
            {
                case filter_t::NONE:
                    filterType = Ogre::TFO_NONE;
                    break;

                case filter_t::LINEAR:
                    filterType = Ogre::TFO_BILINEAR;
                    break;

                case filter_t::ANISOTROPIC:
                    filterType = Ogre::TFO_ANISOTROPIC;
                    break;
            }

            Ogre::Pass* const pass = tech->getPass(0);
            m_texture->bind(pass, "image", filterType);

            pass->getVertexProgramParameters()->setNamedConstant("u_orientation", orientationIndex);

            auto fpParams = pass->getFragmentProgramParameters();
            fpParams->setNamedConstant("u_orientation", orientationIndex);
            fpParams->setNamedConstant("u_enableAlpha", static_cast<int>(_enableTransparency));
        }
    }

    if(m_displayBorder)
    {
        if(m_border != nullptr)
        {
            m_sceneManager->destroyManualObject(m_border);
            m_border = nullptr;
        }

        // Create the border.
        m_border = m_sceneManager->createManualObject(m_slicePlaneName + "_Border");
        m_border->estimateVertexCount(5);
        m_border->begin(m_borderMaterial, Ogre::RenderOperation::OT_LINE_STRIP);

        if(m_orientation == OrientationMode::X_AXIS)
        {
            m_border->position(Ogre::Vector3(0.F, -m_height / 2.F, -m_width / 2.F));
            m_border->position(Ogre::Vector3(0.F, m_height / 2.F, -m_width / 2.F));
            m_border->position(Ogre::Vector3(0.F, m_height / 2.F, m_width / 2.F));
            m_border->position(Ogre::Vector3(0.F, -m_height / 2.F, m_width / 2.F));
            m_border->position(Ogre::Vector3(0.F, -m_height / 2.F, -m_width / 2.F));
            m_border->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(0.F, -m_height / 2.F, -m_width / 2.F),
                    Ogre::Vector3(0.F, m_height / 2.F, m_width / 2.F)
                )
            );
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            m_border->position(Ogre::Vector3(-m_width / 2.F, 0.F, -m_height / 2.F));
            m_border->position(Ogre::Vector3(m_width / 2.F, 0.F, -m_height / 2.F));
            m_border->position(Ogre::Vector3(m_width / 2.F, 0.F, m_height / 2.F));
            m_border->position(Ogre::Vector3(-m_width / 2.F, 0.F, m_height / 2.F));
            m_border->position(Ogre::Vector3(-m_width / 2.F, 0.F, -m_height / 2.F));
            m_border->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-m_height / 2.F, 0.F, -m_width / 2.F),
                    Ogre::Vector3(m_height / 2.F, 0.F, m_width / 2.F)
                )
            );
        }
        else
        {
            m_border->position(Ogre::Vector3(-m_width / 2.F, -m_height / 2.F, 0.F));
            m_border->position(Ogre::Vector3(m_width / 2.F, -m_height / 2.F, 0.F));
            m_border->position(Ogre::Vector3(m_width / 2.F, m_height / 2.F, 0.F));
            m_border->position(Ogre::Vector3(-m_width / 2.F, m_height / 2.F, 0.F));
            m_border->position(Ogre::Vector3(-m_width / 2.F, -m_height / 2.F, 0.F));
            m_border->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-m_height / 2.F, -m_width / 2.F, 0.F),
                    Ogre::Vector3(m_height / 2.F, m_width / 2.F, 0.F)
                )
            );
        }

        m_border->end();

        if(m_orientation == OrientationMode::X_AXIS)
        {
            m_borderMaterial->setAmbient(Ogre::ColourValue::Red);
            m_borderMaterial->setDiffuse(Ogre::ColourValue::Red);
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            m_borderMaterial->setAmbient(Ogre::ColourValue::Green);
            m_borderMaterial->setDiffuse(Ogre::ColourValue::Green);
        }
        else
        {
            m_borderMaterial->setAmbient(Ogre::ColourValue::Blue);
            m_borderMaterial->setDiffuse(Ogre::ColourValue::Blue);
        }

        m_planeSceneNode->attachObject(m_border);
    }

    this->updatePosition();
}

//-----------------------------------------------------------------------------

void Plane::updatePosition()
{
    m_planeSceneNode->setPosition(m_origin);

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_planeSceneNode->translate(0, m_height / 2, m_width / 2);
            break;

        case OrientationMode::Y_AXIS:
            m_planeSceneNode->translate(m_width / 2, 0, m_height / 2);
            break;

        case OrientationMode::Z_AXIS:
            m_planeSceneNode->translate(m_width / 2, m_height / 2, 0);
            break;
    }
}

//-----------------------------------------------------------------------------

void Plane::setTFData(const viz::scene3d::TransferFunction& _tfTexture)
{
    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("Technique is not set", tech);

        if(viz::scene3d::helper::Shading::isColorTechnique(*tech))
        {
            Ogre::Pass* pass = tech->getPass(0);
            _tfTexture.bind(pass, "tfTexture", pass->getFragmentProgramParameters());
        }
    }
}

//------------------------------------------------------------------------------

void Plane::setEntityOpacity(float _f)
{
    m_entityOpacity = _f;

    Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    Ogre::Technique* tech = m_texMaterial->getTechnique(0);
    SIGHT_ASSERT("Technique is not set", tech);

    if(viz::scene3d::helper::Shading::isColorTechnique(*tech)
       && !viz::scene3d::helper::Shading::isPeelTechnique(*tech))
    {
        Ogre::Pass* pass = tech->getPass(0);

        // We don't want a depth check if we have non-OIT transparency
        const bool needDepthCheck = (m_entityOpacity - 1.F) < std::numeric_limits<float>::epsilon();
        pass->setDepthCheckEnabled(needDepthCheck);
    }
}

//------------------------------------------------------------------------------

void Plane::setVisible(bool _visible)
{
    m_planeSceneNode->setVisible(_visible, true);
}

//------------------------------------------------------------------------------

void Plane::changeSlice(float sliceIndex)
{
    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("Technique is not set", tech);

        if(viz::scene3d::helper::Shading::isColorTechnique(*tech))
        {
            Ogre::Pass* pass = tech->getPass(0);

            SIGHT_ASSERT("Can't find Ogre pass", pass);

            pass->getFragmentProgramParameters()->setNamedConstant("u_slice", sliceIndex);
        }
    }

    // as close as possible from 1, but smaller.
    const float relativePosition = std::clamp(sliceIndex, 0.F, 0.999999999999999F);

    this->updatePosition();
    Ogre::Real distance = relativePosition * m_depth;

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_planeSceneNode->translate(distance, 0, 0);
            break;

        case OrientationMode::Y_AXIS:
            m_planeSceneNode->translate(0, distance, 0);
            break;

        case OrientationMode::Z_AXIS:
            m_planeSceneNode->translate(0, 0, distance);
            break;
    }
}

//-----------------------------------------------------------------------------

Ogre::MovablePlane Plane::setDimensions(const Ogre::Vector3& _spacing)
{
    auto tex_width  = static_cast<Ogre::Real>(m_texture->width());
    auto tex_height = static_cast<Ogre::Real>(m_texture->height());
    auto tex_depth  = static_cast<Ogre::Real>(m_texture->depth());

    Ogre::MovablePlane plane(Ogre::Vector3::ZERO, 0);
    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_width  = tex_depth * _spacing[2];
            m_height = tex_height * _spacing[1];
            m_depth  = tex_width * _spacing[0];
            break;

        case OrientationMode::Y_AXIS:
            m_width  = tex_width * _spacing[0];
            m_height = tex_depth * _spacing[2];
            m_depth  = tex_height * _spacing[1];
            break;

        case OrientationMode::Z_AXIS:
            m_width  = tex_width * _spacing[0];
            m_height = tex_height * _spacing[1];
            m_depth  = tex_depth * _spacing[2];
            break;
    }

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            plane = Ogre::MovablePlane(Ogre::Vector3::UNIT_X, 0);
            break;

        case OrientationMode::Y_AXIS:
            plane = Ogre::MovablePlane(Ogre::Vector3::UNIT_Y, 0);
            break;

        case OrientationMode::Z_AXIS:
            plane = Ogre::MovablePlane(Ogre::Vector3::UNIT_Z, 0);
            break;
    }

    return plane;
}

//-----------------------------------------------------------------------------

const Ogre::MovableObject* Plane::getMovableObject() const
{
    if(m_sceneManager->hasEntity(m_entityName))
    {
        return m_sceneManager->getEntity(m_entityName);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void Plane::setQueryFlags(std::uint32_t _flags)
{
    m_sceneManager->getEntity(m_entityName)->setQueryFlags(_flags);
}

//-----------------------------------------------------------------------------

void Plane::setRenderQueuerGroupAndPriority(std::uint8_t _groupId, std::uint16_t _priority)
{
    m_sceneManager->getEntity(m_entityName)->setRenderQueueGroupAndPriority(_groupId, _priority);
}

//-----------------------------------------------------------------------------
std::array<Ogre::Vector3, 4> Plane::computeCross(const Ogre::Vector3& _center, const Ogre::Vector3& _imageOrigin) const
{
    std::array<Ogre::Vector3, 4> crossLines;

    switch(this->getOrientationMode())
    {
        case sight::viz::scene3d::Plane::OrientationMode::X_AXIS:
            crossLines[0] = {_center.x, 0 + _imageOrigin.y, _center.z};
            crossLines[1] = {_center.x, m_height + _imageOrigin.y, _center.z};
            crossLines[2] = {_center.x, _center.y, 0 + _imageOrigin.z};
            crossLines[3] = {_center.x, _center.y, m_width + _imageOrigin.z};
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Y_AXIS:
            crossLines[0] = {0 + _imageOrigin.x, _center.y, _center.z};
            crossLines[1] = {m_width + _imageOrigin.x, _center.y, _center.z};
            crossLines[2] = {_center.x, _center.y, 0 + _imageOrigin.z};
            crossLines[3] = {_center.x, _center.y, m_height + _imageOrigin.z};
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Z_AXIS:
            crossLines[0] = {0 + _imageOrigin.x, _center.y, _center.z};
            crossLines[1] = {m_width + _imageOrigin.x, _center.y, _center.z};
            crossLines[2] = {_center.x, 0 + _imageOrigin.y, _center.z};
            crossLines[3] = {_center.x, m_height + _imageOrigin.y, _center.z};
            break;

        default:
            SIGHT_ASSERT("Unreachable code", false);
    }

    return crossLines;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
