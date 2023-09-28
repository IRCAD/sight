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

#include "viz/scene3d/Plane.hpp"

#include <viz/scene3d/helper/ManualObject.hpp>
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
    const core::tools::id::type& _negatoId,
    Ogre::SceneNode* _parentSceneNode,
    Ogre::SceneManager* _sceneManager,
    viz::scene3d::Texture::sptr _tex,
    filter_t _filtering,
    bool _displayBorder,
    bool _displayOtherPlanes,
    float _entityOpacity
) :
    m_border({.shape = nullptr, .material = nullptr, .enabled = _displayBorder}),
    m_slicesCross({.shape = nullptr, .material = nullptr, .enabled = _displayOtherPlanes}),
    m_filtering(_filtering),
    m_texture(std::move(_tex)),
    m_sceneManager(_sceneManager),
    m_parentSceneNode(_parentSceneNode),
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

    if(m_border.enabled)
    {
        const auto material = materialMgr.getByName("BasicAmbient", RESOURCE_GROUP);
        m_border.material = material->clone(m_slicePlaneName + "_BorderMaterial");
    }

    if(m_slicesCross.enabled)
    {
        const auto material = materialMgr.getByName("BasicAmbientVT", RESOURCE_GROUP);
        m_slicesCross.material = material->clone(m_slicePlaneName + "_OthersMaterial");
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

    if(m_border.shape != nullptr)
    {
        m_sceneManager->destroyManualObject(m_border.shape);
    }

    if(m_texMaterial)
    {
        Ogre::MaterialManager::getSingleton().remove(m_texMaterial);
    }

    if(m_slicesCross.shape != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_slicesCross.material);
    }

    if(m_border.material)
    {
        Ogre::MaterialManager::getSingleton().remove(m_border.material);
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
            m_size[0],
            m_size[1],
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
            m_size[0],
            m_size[1]
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

    if(m_border.enabled)
    {
        if(m_border.shape != nullptr)
        {
            m_sceneManager->destroyManualObject(m_border.shape);
            m_border.shape = nullptr;
        }

        // Create the border.
        m_border.shape = m_sceneManager->createManualObject(m_slicePlaneName + "_Border");
        m_border.shape->estimateVertexCount(5);
        m_border.shape->begin(m_border.material, Ogre::RenderOperation::OT_LINE_STRIP);

        if(m_orientation == OrientationMode::X_AXIS)
        {
            m_border.shape->position(Ogre::Vector3(0.F, -m_size[1] / 2.F, -m_size[0] / 2.F));
            m_border.shape->position(Ogre::Vector3(0.F, m_size[1] / 2.F, -m_size[0] / 2.F));
            m_border.shape->position(Ogre::Vector3(0.F, m_size[1] / 2.F, m_size[0] / 2.F));
            m_border.shape->position(Ogre::Vector3(0.F, -m_size[1] / 2.F, m_size[0] / 2.F));
            m_border.shape->position(Ogre::Vector3(0.F, -m_size[1] / 2.F, -m_size[0] / 2.F));
            m_border.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(0.F, -m_size[1] / 2.F, -m_size[0] / 2.F),
                    Ogre::Vector3(0.F, m_size[1] / 2.F, m_size[0] / 2.F)
                )
            );
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, 0.F, -m_size[1] / 2.F));
            m_border.shape->position(Ogre::Vector3(m_size[0] / 2.F, 0.F, -m_size[1] / 2.F));
            m_border.shape->position(Ogre::Vector3(m_size[0] / 2.F, 0.F, m_size[1] / 2.F));
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, 0.F, m_size[1] / 2.F));
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, 0.F, -m_size[1] / 2.F));
            m_border.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-m_size[1] / 2.F, 0.F, -m_size[0] / 2.F),
                    Ogre::Vector3(m_size[1] / 2.F, 0.F, m_size[0] / 2.F)
                )
            );
        }
        else
        {
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, -m_size[1] / 2.F, 0.F));
            m_border.shape->position(Ogre::Vector3(m_size[0] / 2.F, -m_size[1] / 2.F, 0.F));
            m_border.shape->position(Ogre::Vector3(m_size[0] / 2.F, m_size[1] / 2.F, 0.F));
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, m_size[1] / 2.F, 0.F));
            m_border.shape->position(Ogre::Vector3(-m_size[0] / 2.F, -m_size[1] / 2.F, 0.F));
            m_border.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-m_size[1] / 2.F, -m_size[0] / 2.F, 0.F),
                    Ogre::Vector3(m_size[1] / 2.F, m_size[0] / 2.F, 0.F)
                )
            );
        }

        m_border.shape->end();

        if(m_orientation == OrientationMode::X_AXIS)
        {
            m_border.material->setAmbient(Ogre::ColourValue::Red);
            m_border.material->setDiffuse(Ogre::ColourValue::Red);
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            m_border.material->setAmbient(Ogre::ColourValue::Green);
            m_border.material->setDiffuse(Ogre::ColourValue::Green);
        }
        else
        {
            m_border.material->setAmbient(Ogre::ColourValue::Blue);
            m_border.material->setDiffuse(Ogre::ColourValue::Blue);
        }

        m_planeSceneNode->attachObject(m_border.shape);
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
            m_planeSceneNode->translate(0, m_size[1] / 2, m_size[0] / 2);
            break;

        case OrientationMode::Y_AXIS:
            m_planeSceneNode->translate(m_size[0] / 2, 0, m_size[1] / 2);
            break;

        case OrientationMode::Z_AXIS:
            m_planeSceneNode->translate(m_size[0] / 2, m_size[1] / 2, 0);
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

void Plane::changeSlice(const std::array<float, 3>& _slicesIndex)
{
    const auto currentSlice                      = _slicesIndex[static_cast<std::size_t>(m_orientation)];
    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("Technique is not set", tech);

        if(viz::scene3d::helper::Shading::isColorTechnique(*tech))
        {
            Ogre::Pass* pass = tech->getPass(0);

            SIGHT_ASSERT("Can't find Ogre pass", pass);

            pass->getFragmentProgramParameters()->setNamedConstant("u_slice", currentSlice);
        }
    }

    // as close as possible from 1, but smaller.
    const float relativePosition = std::clamp(currentSlice, 0.F, 0.999999999999999F);

    this->updatePosition();
    Ogre::Real distance = relativePosition * m_size[2];

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

    // Display a cross for the two other planes
    if(m_slicesCross.enabled)
    {
        if(m_slicesCross.shape != nullptr)
        {
            m_sceneManager->destroyManualObject(m_slicesCross.shape);
            m_slicesCross.shape = nullptr;
        }

        // Create the border.
        m_slicesCross.shape = m_sceneManager->createManualObject(m_slicePlaneName + "_Others");
        m_slicesCross.shape->estimateVertexCount(4);
        m_slicesCross.shape->begin(m_slicesCross.material, Ogre::RenderOperation::OT_LINE_LIST);

        const auto halfWidth  = m_size[0] * .5F;
        const auto halfHeight = m_size[1] * .5F;

        const auto dashLength = std::max(std::max(m_size[0], m_size[1]), m_size[2]) / 100.F;
        using sight::viz::scene3d::helper::ManualObject;
        if(m_orientation == OrientationMode::X_AXIS)
        {
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(0.F, _slicesIndex[1] * m_size[1] - halfHeight, -halfWidth),
                Ogre::Vector3(0.F, _slicesIndex[1] * m_size[1] - halfHeight, +halfWidth),
                dashLength,
                dashLength,
                Ogre::ColourValue::Green
            );
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(0.F, -halfHeight, _slicesIndex[2] * m_size[0] - halfWidth),
                Ogre::Vector3(0.F, halfHeight, _slicesIndex[2] * m_size[0] - halfWidth),
                dashLength,
                dashLength,
                Ogre::ColourValue::Blue
            );

            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(0.F, -halfHeight, -halfWidth),
                    Ogre::Vector3(0.F, halfHeight, halfWidth)
                )
            );
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(_slicesIndex[0] * m_size[0] - halfWidth, 0.F, -halfHeight),
                Ogre::Vector3(_slicesIndex[0] * m_size[0] - halfWidth, 0.F, halfHeight),
                dashLength,
                dashLength,
                Ogre::ColourValue::Red
            );
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(-halfWidth, 0.F, _slicesIndex[2] * m_size[1] - halfHeight),
                Ogre::Vector3(halfWidth, 0.F, _slicesIndex[2] * m_size[1] - halfHeight),
                dashLength,
                dashLength,
                Ogre::ColourValue::Blue
            );
            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-halfHeight, 0.F, -halfWidth),
                    Ogre::Vector3(halfHeight, 0.F, halfWidth)
                )
            );
        }
        else
        {
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(_slicesIndex[0] * m_size[0] - halfWidth, -halfHeight, 0.F),
                Ogre::Vector3(_slicesIndex[0] * m_size[0] - halfWidth, halfHeight, 0.F),
                dashLength,
                dashLength,
                Ogre::ColourValue::Red
            );
            ManualObject::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(-halfWidth, _slicesIndex[1] * m_size[1] - halfHeight, 0.F),
                Ogre::Vector3(halfWidth, _slicesIndex[1] * m_size[1] - halfHeight, 0.F),
                dashLength,
                dashLength,
                Ogre::ColourValue::Green
            );

            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-halfHeight, -halfWidth, 0.F),
                    Ogre::Vector3(halfHeight, halfWidth, 0.F)
                )
            );
        }

        m_slicesCross.shape->end();

        m_slicesCross.material->setAmbient(Ogre::ColourValue::White);
        m_slicesCross.material->setDiffuse(Ogre::ColourValue::White);

        m_planeSceneNode->attachObject(m_slicesCross.shape);
    }
}

//-----------------------------------------------------------------------------

Ogre::MovablePlane Plane::setDimensions(const Ogre::Vector3& _spacing)
{
    auto tex_width  = std::max(static_cast<Ogre::Real>(m_texture->width()) - 1.F, 0.F);
    auto tex_height = std::max(static_cast<Ogre::Real>(m_texture->height()) - 1.F, 0.F);
    auto tex_depth  = std::max(static_cast<Ogre::Real>(m_texture->depth()) - 1.F, 0.F);

    Ogre::MovablePlane plane(Ogre::Vector3::ZERO, 0);
    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_size[0] = tex_depth * _spacing[2];
            m_size[1] = tex_height * _spacing[1];
            m_size[2] = tex_width * _spacing[0];
            break;

        case OrientationMode::Y_AXIS:
            m_size[0] = tex_width * _spacing[0];
            m_size[1] = tex_depth * _spacing[2];
            m_size[2] = tex_height * _spacing[1];
            break;

        case OrientationMode::Z_AXIS:
            m_size[0] = tex_width * _spacing[0];
            m_size[1] = tex_height * _spacing[1];
            m_size[2] = tex_depth * _spacing[2];
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
            crossLines[1] = {_center.x, m_size[1] + _imageOrigin.y, _center.z};
            crossLines[2] = {_center.x, _center.y, 0 + _imageOrigin.z};
            crossLines[3] = {_center.x, _center.y, m_size[0] + _imageOrigin.z};
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Y_AXIS:
            crossLines[0] = {0 + _imageOrigin.x, _center.y, _center.z};
            crossLines[1] = {m_size[0] + _imageOrigin.x, _center.y, _center.z};
            crossLines[2] = {_center.x, _center.y, 0 + _imageOrigin.z};
            crossLines[3] = {_center.x, _center.y, m_size[1] + _imageOrigin.z};
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Z_AXIS:
            crossLines[0] = {0 + _imageOrigin.x, _center.y, _center.z};
            crossLines[1] = {m_size[0] + _imageOrigin.x, _center.y, _center.z};
            crossLines[2] = {_center.x, 0 + _imageOrigin.y, _center.z};
            crossLines[3] = {_center.x, m_size[1] + _imageOrigin.y, _center.z};
            break;

        default:
            SIGHT_ASSERT("Unreachable code", false);
    }

    return crossLines;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
