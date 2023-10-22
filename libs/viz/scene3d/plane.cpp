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

#include "viz/scene3d/plane.hpp"

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/shading.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

plane::plane(
    const core::tools::id::type& _negato_id,
    Ogre::SceneNode* _parent_scene_node,
    Ogre::SceneManager* _scene_manager,
    viz::scene3d::texture::sptr _tex,
    filter_t _filtering,
    bool _display_border,
    bool _display_other_planes,
    float _entity_opacity
) :
    m_border({.shape = nullptr, .material = nullptr, .enabled = _display_border}),
    m_slicesCross({.shape = nullptr, .material = nullptr, .enabled = _display_other_planes}),
    m_filtering(_filtering),
    m_texture(std::move(_tex)),
    m_sceneManager(_scene_manager),
    m_parentSceneNode(_parent_scene_node),
    m_entityOpacity(_entity_opacity)
{
    // We need an internal counter to avoid naming conflicts
    static std::uint32_t count = 0;
    m_slicePlaneName = _negato_id + "_Mesh" + std::to_string(count);
    m_entityName     = _negato_id + "_Entity" + std::to_string(count);
    m_sceneNodeName  = _negato_id + "_SceneNode" + std::to_string(count++);

    // Creates the parent's child scene node positionned at (0; 0; 0)
    m_planeSceneNode = m_parentSceneNode->createChildSceneNode(m_sceneNodeName);

    auto& material_mgr            = Ogre::MaterialManager::getSingleton();
    Ogre::MaterialPtr default_mat = material_mgr.getByName("Negato", RESOURCE_GROUP);
    SIGHT_ASSERT("Default material not found, the 'material' module may not be loaded.", default_mat);

    // If the texture material exists, delete it.
    if(m_texMaterial)
    {
        material_mgr.remove(m_texMaterial);
        m_texMaterial.reset();
    }

    m_texMaterial = default_mat->clone(m_slicePlaneName + "_TextMaterial");

    const Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    if(m_border.enabled)
    {
        const auto material = material_mgr.getByName("BasicAmbient", RESOURCE_GROUP);
        m_border.material = material->clone(m_slicePlaneName + "_BorderMaterial");
    }

    if(m_slicesCross.enabled)
    {
        const auto material = material_mgr.getByName("BasicAmbientVT", RESOURCE_GROUP);
        m_slicesCross.material = material->clone(m_slicePlaneName + "_OthersMaterial");
    }
}

//-----------------------------------------------------------------------------

plane::~plane()
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

void plane::update(
    OrientationMode _orientation,
    const Ogre::Vector3& _spacing,
    const Ogre::Vector3& _origin,
    bool _enable_transparency
)
{
    m_orientation = _orientation;
    m_origin      = _origin;

    Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();

    // First delete mesh if it already exists.
    if(mesh_manager.resourceExists(m_slicePlaneName, RESOURCE_GROUP))
    {
        mesh_manager.remove(m_slicePlaneName, RESOURCE_GROUP);
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
        m_slicePlane = mesh_manager.createPlane(
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
        m_slicePlane = mesh_manager.createPlane(
            m_slicePlaneName,
            viz::scene3d::RESOURCE_GROUP,
            plane,
            m_size[0],
            m_size[1]
        );
    }

    // Entity creation.
    Ogre::Entity* plane_entity = m_sceneManager->createEntity(m_entityName, m_slicePlane);
    plane_entity->setMaterial(m_texMaterial);
    m_planeSceneNode->attachObject(plane_entity);

    const int orientation_index = static_cast<int>(m_orientation);

    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        if(viz::scene3d::helper::shading::isColorTechnique(*tech))
        {
            // Sets the texture filtering in the current texture unit state according to the negato's interpolation flag
            Ogre::TextureFilterOptions filter_type = Ogre::TFO_NONE;
            switch(m_filtering)
            {
                case filter_t::NONE:
                    filter_type = Ogre::TFO_NONE;
                    break;

                case filter_t::LINEAR:
                    filter_type = Ogre::TFO_BILINEAR;
                    break;

                case filter_t::ANISOTROPIC:
                    filter_type = Ogre::TFO_ANISOTROPIC;
                    break;
            }

            Ogre::Pass* const pass = tech->getPass(0);
            m_texture->bind(pass, "image", filter_type);

            pass->getVertexProgramParameters()->setNamedConstant("u_orientation", orientation_index);

            auto fp_params = pass->getFragmentProgramParameters();
            fp_params->setNamedConstant("u_orientation", orientation_index);
            fp_params->setNamedConstant("u_enableAlpha", static_cast<int>(_enable_transparency));
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

void plane::updatePosition()
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

void plane::setTFData(const viz::scene3d::transfer_function& _tf_texture)
{
    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        if(viz::scene3d::helper::shading::isColorTechnique(*tech))
        {
            Ogre::Pass* pass = tech->getPass(0);
            _tf_texture.bind(pass, "tfTexture", pass->getFragmentProgramParameters());
        }
    }
}

//------------------------------------------------------------------------------

void plane::setEntityOpacity(float _f)
{
    m_entityOpacity = _f;

    Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    Ogre::Technique* tech = m_texMaterial->getTechnique(0);
    SIGHT_ASSERT("technique is not set", tech);

    if(viz::scene3d::helper::shading::isColorTechnique(*tech)
       && !viz::scene3d::helper::shading::isPeelTechnique(*tech))
    {
        Ogre::Pass* pass = tech->getPass(0);

        // We don't want a depth check if we have non-OIT transparency
        const bool need_depth_check = (m_entityOpacity - 1.F) < std::numeric_limits<float>::epsilon();
        pass->setDepthCheckEnabled(need_depth_check);
    }
}

//------------------------------------------------------------------------------

void plane::setVisible(bool _visible)
{
    m_planeSceneNode->setVisible(_visible, true);
}

//------------------------------------------------------------------------------

void plane::changeSlice(const std::array<float, 3>& _slices_index)
{
    const auto current_slice                     = _slices_index[static_cast<std::size_t>(m_orientation)];
    const Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        if(viz::scene3d::helper::shading::isColorTechnique(*tech))
        {
            Ogre::Pass* pass = tech->getPass(0);

            SIGHT_ASSERT("Can't find Ogre pass", pass);

            pass->getFragmentProgramParameters()->setNamedConstant("u_slice", current_slice);
        }
    }

    // as close as possible from 1, but smaller.
    const float relative_position = std::clamp(current_slice, 0.F, 0.999999999999999F);

    this->updatePosition();
    Ogre::Real distance = relative_position * m_size[2];

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

        const auto half_width  = m_size[0] * .5F;
        const auto half_height = m_size[1] * .5F;

        const auto dash_length = std::max(std::max(m_size[0], m_size[1]), m_size[2]) / 100.F;
        using sight::viz::scene3d::helper::manual_object;
        if(m_orientation == OrientationMode::X_AXIS)
        {
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(0.F, _slices_index[1] * m_size[1] - half_height, -half_width),
                Ogre::Vector3(0.F, _slices_index[1] * m_size[1] - half_height, +half_width),
                dash_length,
                dash_length,
                Ogre::ColourValue::Green
            );
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(0.F, -half_height, _slices_index[2] * m_size[0] - half_width),
                Ogre::Vector3(0.F, half_height, _slices_index[2] * m_size[0] - half_width),
                dash_length,
                dash_length,
                Ogre::ColourValue::Blue
            );

            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(0.F, -half_height, -half_width),
                    Ogre::Vector3(0.F, half_height, half_width)
                )
            );
        }
        else if(m_orientation == OrientationMode::Y_AXIS)
        {
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, 0.F, -half_height),
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, 0.F, half_height),
                dash_length,
                dash_length,
                Ogre::ColourValue::Red
            );
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(-half_width, 0.F, _slices_index[2] * m_size[1] - half_height),
                Ogre::Vector3(half_width, 0.F, _slices_index[2] * m_size[1] - half_height),
                dash_length,
                dash_length,
                Ogre::ColourValue::Blue
            );
            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-half_height, 0.F, -half_width),
                    Ogre::Vector3(half_height, 0.F, half_width)
                )
            );
        }
        else
        {
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, -half_height, 0.F),
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, half_height, 0.F),
                dash_length,
                dash_length,
                Ogre::ColourValue::Red
            );
            manual_object::drawDashedLine(
                m_slicesCross.shape,
                Ogre::Vector3(-half_width, _slices_index[1] * m_size[1] - half_height, 0.F),
                Ogre::Vector3(half_width, _slices_index[1] * m_size[1] - half_height, 0.F),
                dash_length,
                dash_length,
                Ogre::ColourValue::Green
            );

            m_slicesCross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-half_height, -half_width, 0.F),
                    Ogre::Vector3(half_height, half_width, 0.F)
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

Ogre::MovablePlane plane::setDimensions(const Ogre::Vector3& _spacing)
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

const Ogre::MovableObject* plane::getMovableObject() const
{
    if(m_sceneManager->hasEntity(m_entityName))
    {
        return m_sceneManager->getEntity(m_entityName);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void plane::setQueryFlags(std::uint32_t _flags)
{
    m_sceneManager->getEntity(m_entityName)->setQueryFlags(_flags);
}

//-----------------------------------------------------------------------------

void plane::setRenderQueuerGroupAndPriority(std::uint8_t _group_id, std::uint16_t _priority)
{
    m_sceneManager->getEntity(m_entityName)->setRenderQueueGroupAndPriority(_group_id, _priority);
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3, 4> plane::computeCross(const Ogre::Vector3& _center, const Ogre::Vector3& _image_origin) const
{
    std::array<Ogre::Vector3, 4> cross_lines;

    switch(this->getOrientationMode())
    {
        case sight::viz::scene3d::plane::OrientationMode::X_AXIS:
            cross_lines[0] = {_center.x, 0 + _image_origin.y, _center.z};
            cross_lines[1] = {_center.x, m_size[1] + _image_origin.y, _center.z};
            cross_lines[2] = {_center.x, _center.y, 0 + _image_origin.z};
            cross_lines[3] = {_center.x, _center.y, m_size[0] + _image_origin.z};
            break;

        case sight::viz::scene3d::plane::OrientationMode::Y_AXIS:
            cross_lines[0] = {0 + _image_origin.x, _center.y, _center.z};
            cross_lines[1] = {m_size[0] + _image_origin.x, _center.y, _center.z};
            cross_lines[2] = {_center.x, _center.y, 0 + _image_origin.z};
            cross_lines[3] = {_center.x, _center.y, m_size[1] + _image_origin.z};
            break;

        case sight::viz::scene3d::plane::OrientationMode::Z_AXIS:
            cross_lines[0] = {0 + _image_origin.x, _center.y, _center.z};
            cross_lines[1] = {m_size[0] + _image_origin.x, _center.y, _center.z};
            cross_lines[2] = {_center.x, 0 + _image_origin.y, _center.z};
            cross_lines[3] = {_center.x, m_size[1] + _image_origin.y, _center.z};
            break;

        default:
            SIGHT_ASSERT("Unreachable code", false);
    }

    return cross_lines;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
