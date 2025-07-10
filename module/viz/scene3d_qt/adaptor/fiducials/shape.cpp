/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include "shape.hpp"

#include <core/com/slots.hxx>
#include <core/compare.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <data/material.hpp>
#include <data/tools/color.hpp>

#include <geometry/data/image.hpp>

#include <viz/scene3d/helper/fiducials.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/ogre.hpp>

#include <algorithm>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

static constexpr std::uint8_t SHAPE_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

//------------------------------------------------------------------------------

shape::shape() noexcept
{
    new_slot(slots::ACTIVATE_SHAPE_TOOL, &shape::activate_shape_tool, this);
}

//------------------------------------------------------------------------------

sight::service::connections_t shape::auto_connections() const
{
    return {
        {s_IMAGE_INOUT, sight::data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_INDEX_MODIFIED_SIG, sight::service::slots::UPDATE},
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_TYPE_MODIFIED_SIG, sight::service::slots::UPDATE},
    };
}

//------------------------------------------------------------------------------

void shape::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_ORIENTATION_CONFIG = CONFIG + "orientation";
    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = CONFIG + "queryFlags";
    static const std::string s_COLOR_CONFIG       = CONFIG + "color";

    m_font_size   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_interactive = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority    = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_color       = config.get<std::string>(s_COLOR_CONFIG, m_color);

    if(const auto& orientation = config.get_optional<std::string>(s_ORIENTATION_CONFIG); orientation)
    {
        if(*orientation == "axial")
        {
            m_axis = axis_t::z_axis;
        }
        else if(*orientation == "frontal")
        {
            m_axis = axis_t::y_axis;
        }
        else if(*orientation == "sagittal")
        {
            m_axis = axis_t::x_axis;
        }
        else
        {
            SIGHT_ASSERT("Unknown orientation, allow values are `axial`, `frontal` and `sagittal`", false);
        }
    }

    if(const auto& hexa_mask = config.get_optional<std::string>(s_QUERY_MASK_CONFIG); hexa_mask)
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + *hexa_mask,
            hexa_mask->length() > 2
            && hexa_mask->substr(0, 2) == "0x"
        );

        m_query_mask = static_cast<std::uint32_t>(std::stoul(*hexa_mask, nullptr, 16));
    }

    if(const auto& hexa_flags = config.get_optional<std::string>(s_QUERY_FLAGS_CONFIG); hexa_flags)
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + *hexa_flags,
            hexa_flags->length() > 2
            && hexa_flags->substr(0, 2) == "0x"
        );

        m_query_flag = static_cast<std::uint32_t>(std::stoul(*hexa_flags, nullptr, 16));
    }
}

//------------------------------------------------------------------------------

void shape::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();

    auto* const root_node = scene_mgr->getRootSceneNode();

    const sight::viz::scene3d::layer::sptr layer = this->layer();

    // Line
    {
        m_line_material_name = gen_id("_lineMaterialName");

        m_line_material = std::make_unique<sight::viz::scene3d::material::standard>(m_line_material_name);
        m_line_material->set_layout(data::mesh::attribute::point_colors);
        m_line_material->set_shading(sight::data::material::shading_t::ambient, layer->num_lights(), false, false);

        // Retrieve the ogre material to change the depth check.
        const Ogre::MaterialPtr ogre_line_material = Ogre::MaterialManager::getSingleton().getByName(
            m_line_material_name,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        SIGHT_ASSERT("'" + m_line_material_name + "' does not exist.", ogre_line_material);
        const Ogre::Technique* const line_tech = ogre_line_material->getTechnique(0);
        SIGHT_ASSERT("No technique found", line_tech);
        Ogre::Pass* const line_pass = line_tech->getPass(0);
        SIGHT_ASSERT("No pass found", line_pass);
        line_pass->setDepthCheckEnabled(false);
        line_pass->setLineWidth(static_cast<float>(*m_line_width));

        m_line_object = scene_mgr->createManualObject(gen_id("line"));
        root_node->attachObject(m_line_object);
    }

    // Sphere
    {
        m_sphere_material_name = gen_id("_sphereMaterialName");

        // Create materials from our wrapper.
        m_sphere_material = std::make_unique<sight::viz::scene3d::material::standard>(m_sphere_material_name);
        m_sphere_material->set_layout(data::mesh::attribute::point_normals | data::mesh::attribute::point_colors);
        m_sphere_material->set_shading(sight::data::material::shading_t::phong, layer->num_lights(), false, false);

        // Retrieve the ogre material to change the depth check.
        const Ogre::MaterialPtr ogre_sphere_material = Ogre::MaterialManager::getSingleton().getByName(
            m_sphere_material_name,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        SIGHT_ASSERT("'" + m_sphere_material_name + "' does not exist.", ogre_sphere_material);
        const Ogre::Technique* const sphere_tech = ogre_sphere_material->getTechnique(0);
        SIGHT_ASSERT("No technique found", sphere_tech);
        Ogre::Pass* const sphere_pass = sphere_tech->getPass(0);
        SIGHT_ASSERT("No pass found", sphere_pass);
        sphere_pass->setDepthCheckEnabled(false);

        m_sphere_object = scene_mgr->createManualObject(gen_id("sphere"));
        m_sphere_object->setQueryFlags(m_query_flag);
        // Render this sphere over all others objects.
        m_sphere_object->setRenderQueueGroup(SHAPE_RQ_GROUP_ID);
        root_node->attachObject(m_sphere_object);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void shape::updating()
{
    FW_PROFILE_AVG("shape::updating", 1);

    this->render_service()->make_current();

    // Save the fiducial sets
    auto image            = m_image.const_lock();
    const auto& fiducials = image->get_fiducials();

    m_line_object->clear();
    m_sphere_object->clear();

    // Get the color
    const Ogre::ColourValue& default_color =
        [this]
        {
            if(m_color.empty())
            {
                return sight::viz::scene3d::helper::scene::generate_color(m_color_index++);
            }

            sight::ui::color_t color;
            sight::data::tools::color::hexa_string_to_rgba(m_color, color);

            return Ogre::ColourValue(color.data());
        }();

    const auto slice_index = sight::data::helper::medical_image::get_slice_index(
        *image,
        m_axis
    ).value_or(0);

    auto predicate = [slice_index](const data::fiducials_series::query_result& _result)
                     {
                         return _result.m_referenced_frame_number.has_value()
                                && !_result.m_referenced_frame_number.value().empty()
                                && (_result.m_referenced_frame_number.value().at(0) - 1) == slice_index;
                     };

    // Get the fiducials to be displayed
    auto query_results = fiducials->query_fiducials(
        predicate,
        data::fiducials_series::shape::shape
    );

    auto* const camera   = this->layer()->get_default_camera();
    auto projection_type = camera->getProjectionType();

    bool visible = this->visible();

    if(projection_type == Ogre::ProjectionType::PT_ORTHOGRAPHIC)
    {
        const auto& spacing = image->spacing();
        sight::viz::scene3d::helper::fiducials::draw_graphic_data(
            query_results,
            m_line_object,
            m_line_material_name,
            default_color,
            {spacing[0], spacing[1]
            },
            visible
        );

        if(m_interactive)
        {
            sight::viz::scene3d::helper::fiducials::draw_graphic_control_points(
                query_results,
                m_sphere_object,
                m_sphere_material_name,
                default_color,
                {spacing[0], spacing[1]},
                static_cast<float>(*m_sphere_radius),
                4,
                visible
            );
        }
    }
    else
    {
        sight::viz::scene3d::helper::fiducials::draw_contour_data(
            query_results,
            m_line_object,
            m_line_material_name,
            default_color,
            visible
        );
        if(m_interactive)
        {
            sight::viz::scene3d::helper::fiducials::draw_contour_control_points(
                query_results,
                m_sphere_object,
                m_sphere_material_name,
                default_color,
                static_cast<float>(*m_sphere_radius),
                4,
                visible
            );
        }
    }

    update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void shape::stopping()
{
    this->render_service()->make_current();

    if(m_line_object != nullptr)
    {
        m_line_object->clear();

        m_line_object->detachFromParent();
        this->get_scene_manager()->destroyManualObject(m_line_object);
        m_line_object = nullptr;
    }

    m_line_material.reset();

    if(m_sphere_object != nullptr)
    {
        m_sphere_object->clear();

        m_sphere_object->detachFromParent();
        this->get_scene_manager()->destroyManualObject(m_sphere_object);
        m_sphere_object = nullptr;
    }

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->remove_interactor(interactor);
    }

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void shape::set_visible(bool _visible)
{
    this->render_service()->make_current();

    m_line_object->setVisible(_visible);
    m_sphere_object->setVisible(_visible);
}

//------------------------------------------------------------------------------

void shape::activate_shape_tool(bool _activate)
{
    m_interactive = _activate;

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->add_interactor(interactor, m_priority);
    }
    else
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->remove_interactor(interactor);
    }
}

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
