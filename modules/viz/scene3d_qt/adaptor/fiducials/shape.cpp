/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <data/material.hpp>
#include <data/tools/color.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/ogre.hpp>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

static constexpr std::uint8_t SHAPE_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

struct shape::ogre_shape final
{
    struct element
    {
        sight::data::fiducials_series::point3 position {};
        Ogre::SceneNode* const node {nullptr};
        Ogre::ManualObject* const sphere {nullptr};
        Ogre::ManualObject* const line {nullptr};
        Ogre::ManualObject* const dashed_line {nullptr};
    };

    std::vector<element> elements;

    Ogre::SceneManager* const scene_manager {nullptr};

    ogre_shape(
        Ogre::SceneManager* const _scene_manager,
        const Ogre::ColourValue& _color,
        float _sphere_radius,
        const sight::data::fiducials_series::fiducial& _fiducial,
        const std::string& _sphere_material_name,
        const std::string& _line_material_name,
        const std::string& _dashed_line_material_name,
        bool _visible,
        const std::string _adaptor_id,
        std::uint32_t _query_flag,
        bool _show_on_all_slices
    ) :
        scene_manager(_scene_manager)
    {
        auto* const root_node = scene_manager->getRootSceneNode();

        // Iterate over point pairs
        // Only construct the sphere for the first point and the line to the following one
        // This will construct the whole contour
        // No post iteration is required as: p[0] === p[p.size() - 1]

        // We at least need 2 points
        if(const auto size = _fiducial.contour_data.size() - 1;
           size > 0 && _fiducial.shape_type == sight::data::fiducials_series::shape::shape)
        {
            elements.reserve(size);

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                const auto& p1 = _fiducial.contour_data[i];
                const auto& p2 = _fiducial.contour_data[i + 1];

                const Ogre::Vector3 v1(
                    static_cast<float>(p1.x),
                    static_cast<float>(p1.y),
                    static_cast<float>(p1.z)
                );

                const Ogre::Vector3 v2(
                    static_cast<float>(p2.x),
                    static_cast<float>(p2.y),
                    static_cast<float>(p2.z)
                );

                const auto id =
                    [&](const std::string& _type)
                    {
                        return _adaptor_id + "_" + _type + std::to_string(i) + "_"
                               + _fiducial.fiducial_identifier;
                    };

                // Create sphere.
                auto* const sphere = scene_manager->createManualObject(id("sphere"));
                SIGHT_ASSERT("Can't create the first entity", sphere);

                sight::viz::scene3d::helper::manual_object::create_sphere(
                    sphere,
                    _sphere_material_name,
                    _color,
                    _sphere_radius
                );

                sphere->setQueryFlags(_query_flag);
                // Render this sphere over all others objects.
                sphere->setRenderQueueGroup(SHAPE_RQ_GROUP_ID);

                Ogre::SceneNode* const node = root_node->createChildSceneNode(id("node"), v1);
                SIGHT_ASSERT("Can't create the first node", node);
                node->attachObject(sphere);

                // Line.
                auto* const line = scene_manager->createManualObject(id("line"));
                SIGHT_ASSERT("Can't create the line", line);
                line->begin(
                    _line_material_name,
                    Ogre::RenderOperation::OT_LINE_LIST,
                    sight::viz::scene3d::RESOURCE_GROUP
                );
                line->colour(_color);
                line->position(v1);
                line->position(v2);
                line->end();
                line->setQueryFlags(0x0);
                root_node->attachObject(line);

                // Dashed line.
                auto* const dashed_line = scene_manager->createManualObject(id("dashedLine"));
                SIGHT_ASSERT("Can't create the dashed line", dashed_line);
                dashed_line->begin(
                    _dashed_line_material_name,
                    Ogre::RenderOperation::OT_LINE_LIST,
                    sight::viz::scene3d::RESOURCE_GROUP
                );
                dashed_line->colour(_color);
                sight::viz::scene3d::helper::manual_object::draw_dashed_line(
                    dashed_line,
                    v1,
                    v2,
                    _sphere_radius,
                    _sphere_radius,
                    _color
                );
                dashed_line->end();
                dashed_line->setQueryFlags(0x0);
                // Render this line over all others objects.
                dashed_line->setRenderQueueGroup(SHAPE_RQ_GROUP_ID);
                root_node->attachObject(dashed_line);

                // Set the visibility.
                sphere->setVisible(_visible || _show_on_all_slices);
                line->setVisible(_visible);
                dashed_line->setVisible(!_visible && _show_on_all_slices);

                elements.emplace_back(element {p1, node, sphere, line, dashed_line});
            }
        }
    }

    ~ogre_shape()
    {
        for(auto& element : elements)
        {
            scene_manager->destroySceneNode(element.node);
            scene_manager->destroyManualObject(element.sphere);
            scene_manager->destroyManualObject(element.line);
            scene_manager->destroyManualObject(element.dashed_line);
        }
    }
};

//------------------------------------------------------------------------------

shape::shape() noexcept
{
    new_slot(slots::ACTIVATE_SHAPE_TOOL, &shape::activate_shape_tool, this);
    new_slot(private_slots::SHOW_ON_CURRENT_SLICE, &shape::show_on_current_slice, this);
}

//------------------------------------------------------------------------------

sight::service::connections_t shape::auto_connections() const
{
    return {
        {s_IMAGE_INOUT, sight::data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_INDEX_MODIFIED_SIG, private_slots::SHOW_ON_CURRENT_SLICE},
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_TYPE_MODIFIED_SIG, private_slots::SHOW_ON_CURRENT_SLICE},
    };
}

//------------------------------------------------------------------------------

void shape::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_ORIENTATION_CONFIG = CONFIG + "orientation";
    static const std::string s_ALL_SLICES_CONFIG  = CONFIG + "show_on_all_slices";
    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_RADIUS_CONFIG      = CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = CONFIG + "queryFlags";
    static const std::string s_COLOR_CONFIG       = CONFIG + "color";

    m_show_on_all_slices = config.get<bool>(s_ALL_SLICES_CONFIG, m_show_on_all_slices);
    m_font_size          = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_sphere_radius      = config.get<float>(s_RADIUS_CONFIG, m_sphere_radius);
    m_interactive        = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority           = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_color              = config.get<std::string>(s_COLOR_CONFIG, m_color);

    if(const auto& orientation = config.get_optional<std::string>(s_ORIENTATION_CONFIG); orientation)
    {
        if(*orientation == "axial")
        {
            m_orientation = orientation_t::z_axis;
        }
        else if(*orientation == "frontal")
        {
            m_orientation = orientation_t::y_axis;
        }
        else if(*orientation == "sagittal")
        {
            m_orientation = orientation_t::x_axis;
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

    const sight::viz::scene3d::layer::sptr layer = this->layer();

    m_sphere_material_name      = this->get_id() + "_sphereMaterialName";
    m_line_material_name        = this->get_id() + "_lineMaterialName";
    m_dashed_line_material_name = this->get_id() + "_dashedLineMaterialName";

    // Create materials from our wrapper.
    // Sphere
    m_sphere_material = std::make_unique<sight::viz::scene3d::material>(
        m_sphere_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_sphere_material->set_has_vertex_color(true);
    m_sphere_material->update_shading_mode(sight::data::material::shading_t::phong, layer->num_lights(), false, false);

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

    // Line
    m_line_material = std::make_unique<sight::viz::scene3d::material>(
        m_line_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_line_material->set_has_vertex_color(true);
    m_line_material->update_shading_mode(sight::data::material::shading_t::ambient, layer->num_lights(), false, false);

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

    // Dashed line
    m_dashed_line_material = std::make_unique<sight::viz::scene3d::material>(
        m_dashed_line_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_dashed_line_material->set_has_vertex_color(true);
    m_dashed_line_material->update_shading_mode(
        sight::data::material::shading_t::ambient,
        layer->num_lights(),
        false,
        false
    );

    // Retrieve the ogre material to change the depth check.
    const Ogre::MaterialPtr ogre_dashed_line_material = Ogre::MaterialManager::getSingleton().getByName(
        m_dashed_line_material_name,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    SIGHT_ASSERT("'" + m_dashed_line_material_name + "' does not exist.", ogre_dashed_line_material);
    const Ogre::Technique* const dashed_tech = ogre_dashed_line_material->getTechnique(0);
    SIGHT_ASSERT("No technique found", dashed_tech);
    Ogre::Pass* const dashed_pass = dashed_tech->getPass(0);
    SIGHT_ASSERT("No pass found", dashed_pass);
    dashed_pass->setDepthCheckEnabled(false);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        layer->add_interactor(interactor, m_priority);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void shape::updating()
{
    this->render_service()->make_current();

    // Destroy previous shapes
    m_shapes.clear();

    // Save the fiducial sets
    const auto& fiducial_sets = m_image.const_lock()->get_fiducials()->get_fiducial_sets();

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

    auto* const scene_manager = this->get_scene_manager();

    m_shapes.reserve(fiducial_sets.size());

    std::for_each(
        fiducial_sets.cbegin(),
        fiducial_sets.cend(),
        [&](const auto& _fiducial_set)
        {
            const Ogre::ColourValue& color =
                _fiducial_set.color
                ? Ogre::ColourValue(
                    (*_fiducial_set.color)[0],
                    (*_fiducial_set.color)[1],
                    (*_fiducial_set.color)[2],
                    (*_fiducial_set.color)[3]
                )
                : default_color;

            const float sphere_radius = _fiducial_set.size.value_or(m_sphere_radius) * (m_interactive ? 2.0F : 1.0F);

            const bool visible = _fiducial_set.visibility.value_or(this->visible());

            std::vector<std::shared_ptr<ogre_shape> > shapes;
            shapes.reserve(_fiducial_set.fiducial_sequence.size());

            std::for_each(
                _fiducial_set.fiducial_sequence.cbegin(),
                _fiducial_set.fiducial_sequence.cend(),
                [&](const auto& _fiducial)
            {
                auto ogre_shape = std::make_shared<shape::ogre_shape>(
                    scene_manager,
                    color,
                    sphere_radius,
                    _fiducial,
                    m_sphere_material_name,
                    m_line_material_name,
                    m_dashed_line_material_name,
                    visible,
                    this->get_id(),
                    m_query_flag,
                    m_show_on_all_slices
                );

                shapes.emplace_back(ogre_shape);
            });

            m_shapes.emplace_back(shapes);
        });

    update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void shape::stopping()
{
    this->render_service()->make_current();

    // Destroy all shapes
    m_shapes.clear();

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
    if(!_visible)
    {
        this->render_service()->make_current();

        for(auto& shapes : m_shapes)
        {
            for(auto& shape : shapes)
            {
                for(auto& element : shape->elements)
                {
                    element.sphere->setVisible(false);
                    element.line->setVisible(false);
                    element.dashed_line->setVisible(false);
                }
            }
        }
    }
    else
    {
        this->show_on_current_slice();
    }
}

//------------------------------------------------------------------------------

void shape::activate_shape_tool(bool _activate)
{
    m_interactive = _activate;
}

//------------------------------------------------------------------------------

void shape::show_on_current_slice()
{
    // Get the current slice position
    const auto& [spacing, slice_position] =
        [this]
        {
            const auto image_series = m_image.const_lock();
            const auto& spacing     = image_series->spacing();
            const auto& origin      = image_series->origin();
            const auto slice_index  = sight::data::helper::medical_image::get_slice_index(
                *image_series,
                m_orientation
            ).value_or(0);

            return std::make_tuple(
                spacing[m_orientation],
                origin[m_orientation] + (double(slice_index) * spacing[m_orientation])
            );
        }();

    this->render_service()->make_current();

    for(auto& shapes : m_shapes)
    {
        for(auto& shape : shapes)
        {
            for(auto& element : shape->elements)
            {
                // Check if the element is on the current slice
                const auto position =
                    m_orientation == orientation_t::z_axis
                    ? element.position.z
                    : m_orientation == orientation_t::y_axis
                    ? element.position.y
                    : element.position.x;

                const auto rounded_position       = static_cast<int>(std::floor(position / spacing));
                const auto rounded_slice_position = static_cast<int>(std::floor(slice_position / spacing));
                const bool visible                = sight::core::is_equal(
                    rounded_position,
                    rounded_slice_position
                );

                // Set the visibility
                element.sphere->setVisible(visible || m_show_on_all_slices);
                element.line->setVisible(visible);

                // Show only the dashed line if we are not on the current slice
                element.dashed_line->setVisible(!visible && m_show_on_all_slices);
            }
        }
    }
}

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
