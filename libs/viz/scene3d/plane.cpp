/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <geometry/data/image.hpp>

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
    const core::id::type& _negato_id,
    Ogre::SceneNode* _parent_scene_node,
    Ogre::SceneManager* _scene_manager,
    viz::scene3d::texture::sptr _tex,
    filter_t _filtering,
    bool _display_border,
    bool _display_other_planes,
    float _entity_opacity
) :
    m_border({.shape       = nullptr, .material = nullptr, .enabled = _display_border}),
    m_slices_cross({.shape = nullptr, .material = nullptr, .enabled = _display_other_planes}),
    m_filtering(_filtering),
    m_texture(std::move(_tex)),
    m_scene_manager(_scene_manager),
    m_parent_scene_node(_parent_scene_node),
    m_entity_opacity(_entity_opacity)
{
    // We need an internal counter to avoid naming conflicts
    static std::uint32_t count = 0;
    m_slice_plane_name = core::id::join(_negato_id, "mesh", std::to_string(count));
    m_entity_name      = core::id::join(_negato_id, "entity", std::to_string(count));
    m_scene_node_name  = core::id::join(_negato_id, "scene_node", std::to_string(count++));

    // Creates the parent's child scene node positionned at (0; 0; 0)
    m_plane_scene_node = m_parent_scene_node->createChildSceneNode(m_scene_node_name);

    const auto plane_material_name = core::id::join(m_slice_plane_name + "plane_material");
    m_plane_material = std::make_unique<viz::scene3d::material::generic>(plane_material_name, "Negato");

    const Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entity_opacity);
    m_plane_material->material()->setDiffuse(diffuse);

    auto& material_mgr = Ogre::MaterialManager::getSingleton();
    if(m_border.enabled)
    {
        const auto material = material_mgr.getByName("BasicAmbient", RESOURCE_GROUP);
        m_border.material = material->clone(m_slice_plane_name + "_BorderMaterial");
    }

    if(m_slices_cross.enabled)
    {
        const auto material = material_mgr.getByName("BasicAmbientVT", RESOURCE_GROUP);
        m_slices_cross.material = material->clone(m_slice_plane_name + "_OthersMaterial");
    }
}

//-----------------------------------------------------------------------------

plane::~plane()
{
    m_plane_scene_node->removeAndDestroyAllChildren();
    m_scene_manager->destroySceneNode(m_plane_scene_node);

    if(m_scene_manager->hasEntity(m_entity_name))
    {
        m_scene_manager->destroyEntity(m_entity_name);
    }

    if(m_slice_plane)
    {
        Ogre::MeshManager::getSingleton().remove(m_slice_plane);
    }

    if(m_border.shape != nullptr)
    {
        m_scene_manager->destroyManualObject(m_border.shape);
    }

    m_plane_material.reset();

    if(m_slices_cross.shape != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_slices_cross.material);
    }

    if(m_border.material)
    {
        Ogre::MaterialManager::getSingleton().remove(m_border.material);
    }
}

//-----------------------------------------------------------------------------

void plane::update(
    axis_t _axis,
    const Ogre::Vector3& _spacing,
    bool _enable_transparency
)
{
    m_axis = _axis;

    Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();

    // First delete mesh if it already exists.
    if(mesh_manager.resourceExists(m_slice_plane_name, RESOURCE_GROUP))
    {
        mesh_manager.remove(m_slice_plane_name, RESOURCE_GROUP);
        m_slice_plane.reset();
    }

    // Save visibility if the plane existed before
    bool visible = true;
    if(m_scene_manager->hasEntity(m_entity_name))
    {
        auto* entity = m_scene_manager->getEntity(m_entity_name);
        SIGHT_ASSERT("Could not find entity " + m_entity_name, entity);
        entity->detachFromParent();
        visible = entity->isVisible();
        m_scene_manager->destroyEntity(m_entity_name);
    }

    Ogre::MovablePlane plane = this->set_dimensions(_spacing);

    // Mesh plane instantiation:
    // Y is the default upVector,
    // so if we want a plane which normal is the Y unit vector we have to create it differently.
    if(m_axis == axis_t::y_axis)
    {
        m_slice_plane = mesh_manager.createPlane(
            m_slice_plane_name,
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
        m_slice_plane = mesh_manager.createPlane(
            m_slice_plane_name,
            viz::scene3d::RESOURCE_GROUP,
            plane,
            m_size[0],
            m_size[1]
        );
    }

    // Entity creation.
    Ogre::Entity* plane_entity = m_scene_manager->createEntity(m_entity_name, m_slice_plane);
    plane_entity->setMaterial(m_plane_material->material());
    m_plane_scene_node->attachObject(plane_entity);

    const int orientation_index = static_cast<int>(m_axis);

    // Sets the texture filtering in the current texture unit state according to the negato's interpolation flag
    Ogre::TextureFilterOptions filter_type = Ogre::TFO_NONE;
    switch(m_filtering)
    {
        case filter_t::none:
            filter_type = Ogre::TFO_NONE;
            break;

        case filter_t::linear:
            filter_type = Ogre::TFO_BILINEAR;
            break;

        case filter_t::anisotropic:
            filter_type = Ogre::TFO_ANISOTROPIC;
            break;
    }

    m_plane_material->set_texture("image", m_texture->get(), filter_type);
    m_plane_material->set_vertex_uniform("u_orientation", orientation_index);
    m_plane_material->set_fragment_uniform("u_window", m_texture->window());
    m_plane_material->set_fragment_uniform("u_orientation", orientation_index);
    m_plane_material->set_fragment_uniform("u_enableAlpha", static_cast<int>(_enable_transparency));

    if(m_border.enabled)
    {
        if(m_border.shape != nullptr)
        {
            m_scene_manager->destroyManualObject(m_border.shape);
            m_border.shape = nullptr;
        }

        // Create the border.
        m_border.shape = m_scene_manager->createManualObject(m_slice_plane_name + "_Border");
        m_border.shape->estimateVertexCount(5);
        m_border.shape->begin(m_border.material, Ogre::RenderOperation::OT_LINE_STRIP);

        if(m_axis == axis_t::x_axis)
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
        else if(m_axis == axis_t::y_axis)
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

        if(m_axis == axis_t::x_axis)
        {
            m_border.material->setAmbient(Ogre::ColourValue::Red);
            m_border.material->setDiffuse(Ogre::ColourValue::Red);
        }
        else if(m_axis == axis_t::y_axis)
        {
            m_border.material->setAmbient(Ogre::ColourValue::Green);
            m_border.material->setDiffuse(Ogre::ColourValue::Green);
        }
        else
        {
            m_border.material->setAmbient(Ogre::ColourValue::Blue);
            m_border.material->setDiffuse(Ogre::ColourValue::Blue);
        }

        m_plane_scene_node->attachObject(m_border.shape);
    }

    // Restore visibility if the plane existed before
    m_plane_scene_node->setVisible(visible, true);

    this->update_position();
}

//-----------------------------------------------------------------------------

void plane::update_position()
{
    m_plane_scene_node->resetToInitialState();

    switch(m_axis)
    {
        case axis_t::x_axis:
            m_plane_scene_node->translate(0, m_size[1] / 2, m_size[0] / 2);
            break;

        case axis_t::y_axis:
            m_plane_scene_node->translate(m_size[0] / 2, 0, m_size[1] / 2);
            break;

        case axis_t::z_axis:
            m_plane_scene_node->translate(m_size[0] / 2, m_size[1] / 2, 0);
            break;
    }
}

//-----------------------------------------------------------------------------

void plane::set_tf_data(const viz::scene3d::transfer_function& _tf_texture)
{
    m_plane_material->set_texture("tfTexture", _tf_texture.get());
    m_plane_material->set_fragment_uniform("u_f3TFWindow", _tf_texture.window);
}

//------------------------------------------------------------------------------

void plane::set_entity_opacity(float _f)
{
    m_entity_opacity = _f;

    Ogre::ColourValue diffuse(1.F, 1.F, 1.F, m_entity_opacity);
    m_plane_material->material()->setDiffuse(diffuse);
}

//------------------------------------------------------------------------------

void plane::set_visible(bool _visible)
{
    m_plane_scene_node->setVisible(_visible, true);
}

//------------------------------------------------------------------------------

void plane::change_slice(const std::array<float, 3>& _slices_index)
{
    const auto current_slice = _slices_index[static_cast<std::size_t>(m_axis)];

    m_plane_material->set_fragment_uniform("u_slice", current_slice);

    // as close as possible from 1, but smaller.
    const float relative_position = std::clamp(current_slice, 0.F, 0.999999999999999F);

    this->update_position();
    Ogre::Real distance = relative_position * m_size[2];

    switch(m_axis)
    {
        case axis_t::x_axis:
            m_plane_scene_node->translate(distance, 0, 0);
            break;

        case axis_t::y_axis:
            m_plane_scene_node->translate(0, distance, 0);
            break;

        case axis_t::z_axis:
            m_plane_scene_node->translate(0, 0, distance);
            break;
    }

    // Display a cross for the two other planes
    if(m_slices_cross.enabled)
    {
        if(m_slices_cross.shape != nullptr)
        {
            m_scene_manager->destroyManualObject(m_slices_cross.shape);
            m_slices_cross.shape = nullptr;
        }

        // Create the border.
        m_slices_cross.shape = m_scene_manager->createManualObject(m_slice_plane_name + "_Others");
        m_slices_cross.shape->estimateVertexCount(4);
        m_slices_cross.shape->begin(m_slices_cross.material, Ogre::RenderOperation::OT_LINE_LIST);

        const auto half_width  = m_size[0] * .5F;
        const auto half_height = m_size[1] * .5F;

        const auto dash_length = std::max(std::max(m_size[0], m_size[1]), m_size[2]) / 100.F;
        using sight::viz::scene3d::helper::manual_object;
        if(m_axis == axis_t::x_axis)
        {
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(0.F, _slices_index[1] * m_size[1] - half_height, -half_width),
                Ogre::Vector3(0.F, _slices_index[1] * m_size[1] - half_height, +half_width),
                dash_length,
                dash_length,
                Ogre::ColourValue::Green
            );
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(0.F, -half_height, _slices_index[2] * m_size[0] - half_width),
                Ogre::Vector3(0.F, half_height, _slices_index[2] * m_size[0] - half_width),
                dash_length,
                dash_length,
                Ogre::ColourValue::Blue
            );

            m_slices_cross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(0.F, -half_height, -half_width),
                    Ogre::Vector3(0.F, half_height, half_width)
                )
            );
        }
        else if(m_axis == axis_t::y_axis)
        {
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, 0.F, -half_height),
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, 0.F, half_height),
                dash_length,
                dash_length,
                Ogre::ColourValue::Red
            );
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(-half_width, 0.F, _slices_index[2] * m_size[1] - half_height),
                Ogre::Vector3(half_width, 0.F, _slices_index[2] * m_size[1] - half_height),
                dash_length,
                dash_length,
                Ogre::ColourValue::Blue
            );
            m_slices_cross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-half_height, 0.F, -half_width),
                    Ogre::Vector3(half_height, 0.F, half_width)
                )
            );
        }
        else
        {
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, -half_height, 0.F),
                Ogre::Vector3(_slices_index[0] * m_size[0] - half_width, half_height, 0.F),
                dash_length,
                dash_length,
                Ogre::ColourValue::Red
            );
            manual_object::draw_dashed_line(
                m_slices_cross.shape,
                Ogre::Vector3(-half_width, _slices_index[1] * m_size[1] - half_height, 0.F),
                Ogre::Vector3(half_width, _slices_index[1] * m_size[1] - half_height, 0.F),
                dash_length,
                dash_length,
                Ogre::ColourValue::Green
            );

            m_slices_cross.shape->setBoundingBox(
                Ogre::AxisAlignedBox(
                    Ogre::Vector3(-half_height, -half_width, 0.F),
                    Ogre::Vector3(half_height, half_width, 0.F)
                )
            );
        }

        m_slices_cross.shape->end();

        m_slices_cross.material->setAmbient(Ogre::ColourValue::White);
        m_slices_cross.material->setDiffuse(Ogre::ColourValue::White);

        m_plane_scene_node->attachObject(m_slices_cross.shape);
    }
}

//-----------------------------------------------------------------------------

Ogre::MovablePlane plane::set_dimensions(const Ogre::Vector3& _spacing)
{
    auto tex_width  = std::max(static_cast<Ogre::Real>(m_texture->width()) - 1.F, 0.F);
    auto tex_height = std::max(static_cast<Ogre::Real>(m_texture->height()) - 1.F, 0.F);
    auto tex_depth  = std::max(static_cast<Ogre::Real>(m_texture->depth()) - 1.F, 0.F);

    Ogre::MovablePlane plane(Ogre::Vector3::ZERO, 0);
    switch(m_axis)
    {
        case axis_t::x_axis:
            m_size[0] = tex_depth * _spacing[2];
            m_size[1] = tex_height * _spacing[1];
            m_size[2] = tex_width * _spacing[0];
            plane     = Ogre::MovablePlane(Ogre::Vector3::UNIT_X, 0);
            break;

        case axis_t::y_axis:
            m_size[0] = tex_width * _spacing[0];
            m_size[1] = tex_depth * _spacing[2];
            m_size[2] = tex_height * _spacing[1];
            plane     = Ogre::MovablePlane(Ogre::Vector3::UNIT_Y, 0);
            break;

        case axis_t::z_axis:
            m_size[0] = tex_width * _spacing[0];
            m_size[1] = tex_height * _spacing[1];
            m_size[2] = tex_depth * _spacing[2];
            plane     = Ogre::MovablePlane(Ogre::Vector3::UNIT_Z, 0);
            break;
    }

    return plane;
}

//-----------------------------------------------------------------------------

const Ogre::MovableObject* plane::get_movable_object() const
{
    if(m_scene_manager->hasEntity(m_entity_name))
    {
        return m_scene_manager->getEntity(m_entity_name);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void plane::set_query_flags(std::uint32_t _flags)
{
    m_scene_manager->getEntity(m_entity_name)->setQueryFlags(_flags);
}

//-----------------------------------------------------------------------------

void plane::set_render_queuer_group_and_priority(std::uint8_t _group_id, std::uint16_t _priority)
{
    m_scene_manager->getEntity(m_entity_name)->setRenderQueueGroupAndPriority(_group_id, _priority);
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3, 4> plane::compute_cross(const Ogre::Vector3& _center, const data::image& _image) const
{
    const auto center   = geometry::data::world_to_image(_image, _center);
    const auto& spacing = _image.spacing();

    std::array<Ogre::Vector3, 4> cross_lines;

    switch(this->axis())
    {
        case axis_t::x_axis:
            cross_lines[0] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                0,
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[1] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                m_size[1],
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[2] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                Ogre::Real(double(center[1]) * spacing[1]),
                0
            };

            cross_lines[3] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                Ogre::Real(double(center[1]) * spacing[1]),
                m_size[0]
            };
            break;

        case axis_t::y_axis:
            cross_lines[0] = {
                0,
                Ogre::Real(double(center[1]) * spacing[1]),
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[1] = {
                m_size[0],
                Ogre::Real(double(center[1]) * spacing[1]),
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[2] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                Ogre::Real(double(center[1]) * spacing[1]),
                0
            };

            cross_lines[3] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                Ogre::Real(double(center[1]) * spacing[1]),
                m_size[1]
            };
            break;

        case axis_t::z_axis:
            cross_lines[0] = {
                0,
                Ogre::Real(double(center[1]) * spacing[1]),
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[1] = {
                m_size[0],
                Ogre::Real(double(center[1]) * spacing[1]),
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[2] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                0,
                Ogre::Real(double(center[2]) * spacing[2])
            };

            cross_lines[3] = {
                Ogre::Real(double(center[0]) * spacing[0]),
                m_size[1],
                Ogre::Real(double(center[2]) * spacing[2])
            };
            break;

        default:
            SIGHT_ASSERT("Unreachable code", false);
    }

    return cross_lines;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
