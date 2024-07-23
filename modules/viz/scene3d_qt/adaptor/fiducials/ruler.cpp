/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "ruler.hpp"

#include "core/runtime/path.hpp"

#include <core/com/slots.hxx>
#include <core/tools/uuid.hpp>

#include <data/tools/color.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/ogre.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

static constexpr std::uint8_t RULER_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

//------------------------------------------------------------------------------

ruler::ruler() noexcept
{
    new_slot(slots::REMOVE_ALL, &ruler::remove_all, this);
    new_slot(slots::ACTIVATE_TOOL, &ruler::activate_tool, this);
    new_slot(slots::REMOVE_FROM_CURRENT_SLICE, &ruler::remove_from_current_slice, this);
    new_slot(private_slots::UPDATE_MODIFIED_RULER, &ruler::update_modified_ruler, this);
    new_slot(private_slots::REMOVE_RULER_OGRE_SET, &ruler::remove_ruler_ogre_set, this);
    new_slot(private_slots::DISPLAY_ON_CURRENT_SLICE, &ruler::display_on_current_slice, this);

    new_signal<signals::void_signal_t>(signals::TOOL_DEACTIVATED);
}

//------------------------------------------------------------------------------

sight::service::connections_t ruler::auto_connections() const
{
    return {
        {s_IMAGE_INOUT, sight::data::object::MODIFIED_SIG, sight::service::slots::UPDATE},
        {s_IMAGE_INOUT, sight::data::image_series::RULER_MODIFIED_SIG, private_slots::UPDATE_MODIFIED_RULER},
        {s_IMAGE_INOUT, sight::data::image_series::FIDUCIAL_REMOVED_SIG, private_slots::REMOVE_RULER_OGRE_SET},
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_INDEX_MODIFIED_SIG,
         private_slots::DISPLAY_ON_CURRENT_SLICE
        },
        {s_IMAGE_INOUT, sight::data::image_series::SLICE_TYPE_MODIFIED_SIG,
         private_slots::DISPLAY_ON_CURRENT_SLICE
        },
    };
}

//------------------------------------------------------------------------------

void ruler::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_ORIENTATION_CONFIG        = CONFIG + "orientation";
    static const std::string s_FONT_SIZE_CONFIG          = CONFIG + "font_size";
    static const std::string s_RADIUS_CONFIG             = CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG        = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG           = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG         = CONFIG + "query_mask";
    static const std::string s_QUERY_FLAGS_CONFIG        = CONFIG + "query_flags";
    static const std::string s_COLOR_CONFIG              = CONFIG + "color";
    static const std::string s_ALWAYS_DISPLAY_ALL_CONFIG = CONFIG + "always_display_all";

    m_font_size          = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_sphere_radius      = config.get<float>(s_RADIUS_CONFIG, m_sphere_radius);
    m_priority           = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_color              = config.get<std::string>(s_COLOR_CONFIG, m_color);
    m_always_display_all = config.get<bool>(s_ALWAYS_DISPLAY_ALL_CONFIG, m_always_display_all);

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

void ruler::starting()
{
    this->initialize();

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
    line_pass->setDepthCheckEnabled(true);

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

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);
}

//------------------------------------------------------------------------------

void ruler::updating()
{
    this->render_service()->make_current();

    const sight::viz::scene3d::layer::csptr layer = this->layer();

    m_sphere_material->update_shading_mode(data::material::shading_t::phong, layer->num_lights(), false, false);
    m_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
    m_dashed_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);

    while(!m_ruler_ogre_sets.empty())
    {
        this->remove_ruler_ogre_set(m_ruler_ogre_sets[0].id);
    }

    const auto& fiducial_sets      = m_image.const_lock()->get_fiducials()->get_fiducial_sets();
    std::size_t fiducial_set_index = 0;

    std::for_each(
        fiducial_sets.cbegin(),
        fiducial_sets.cend(),
        [&](const auto& _fiducial_set)
        {
            std::for_each(
                _fiducial_set.fiducial_sequence.cbegin(),
                _fiducial_set.fiducial_sequence.cend(),
                [&](const auto& _fiducial)
            {
                if(_fiducial.shape_type == data::fiducials_series::shape::ruler)
                {
                    // Get the generated or configured color
                    const Ogre::ColourValue& default_color = this->get_default_color();

                    std::array<float, 4> color = {default_color[0], default_color[1], default_color[2], default_color[3]
                    };

                    if(_fiducial_set.color.has_value())
                    {
                        color = _fiducial_set.color.value();
                    }
                    else
                    {
                        // When the fiducial_set has no registered colors.
                        // This case occurs when fiducials are created outside of the adaptor and not manually.
                        // Then we need to save the default color given by the adaptor in the associated fiducial_set.
                        const auto image = m_image.lock();
                        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
                        {
                            image_series->get_fiducials()->set_color(fiducial_set_index, color);
                        }
                    }

                    const float sphere_radius = _fiducial_set.size.value_or(m_sphere_radius)
                                                * (m_interactive ? 2.0F : 1.0F);

                    bool visible = m_always_display_all ? m_always_display_all : this->is_visible_on_current_slice(
                        {_fiducial.contour_data[0].x, _fiducial.contour_data[0].y, _fiducial.contour_data[0].z
                        },
                        {_fiducial.contour_data[1].x, _fiducial.contour_data[1].y, _fiducial.contour_data[1].z
                        });

                    this->create_ruler_ogre_set(
                        color,
                        sphere_radius,
                        _fiducial.fiducial_uid,
                        {_fiducial.contour_data[0].x, _fiducial.contour_data[0].y, _fiducial.contour_data[0].z},
                        {_fiducial.contour_data[1].x, _fiducial.contour_data[1].y, _fiducial.contour_data[1].z},
                        m_visible && visible
                    );
                }
            });
            fiducial_set_index++;
        });

    this->request_render();
}

//------------------------------------------------------------------------------

void ruler::stopping()
{
    this->render_service()->make_current();

    while(!m_ruler_ogre_sets.empty())
    {
        this->remove_ruler_ogre_set(m_ruler_ogre_sets[0].id);
    }

    m_sphere_material.reset();
    m_line_material.reset();
    m_dashed_line_material.reset();

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->layer()->remove_interactor(interactor);

    m_event_filter = nullptr;
}

//------------------------------------------------------------------------------

void ruler::create_ruler_fiducial(
    const std::array<float, 4> _color,
    const std::optional<std::string> _id,
    const std::array<double, 3> _begin,
    const std::array<double, 3> _end
)
{
    const auto image = m_image.lock();
    if(data::helper::medical_image::check_image_validity(image.get_shared()))
    {
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            data::fiducials_series::fiducial_set fiducial_set;
            std::string frame_of_reference_uid = image_series->get_string_value(
                data::dicom::attribute::Keyword::FrameOfReferenceUID
            );
            if(frame_of_reference_uid.empty())
            {
                // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory
                // according to the DICOM standard anyway.
                frame_of_reference_uid = core::tools::uuid::generate();
                image_series->set_string_value(
                    data::dicom::attribute::Keyword::FrameOfReferenceUID,
                    frame_of_reference_uid
                );
            }

            fiducial_set.frame_of_reference_uid = frame_of_reference_uid;
            data::fiducials_series::fiducial fiducial;
            fiducial.shape_type           = data::fiducials_series::shape::ruler;
            fiducial.fiducial_identifier  = sight::core::tools::uuid::generate();
            fiducial.fiducial_description = "Ruler";
            fiducial.fiducial_uid         = _id.value_or("");
            fiducial.contour_data         = {
                {.x                       = _begin[0], .y = _begin[1], .z = _begin[2]},
                {.x                       = _end[0], .y = _end[1], .z = _end[2]}
            };
            // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too
            // lazy to do that, so I simply get rid of GraphicCoordinatesDataSequence.
            fiducial.graphic_coordinates_data_sequence = std::nullopt;
            fiducial_set.fiducial_sequence.push_back(fiducial);

            // Fill in the referenced_image with the slice index to easily retrieve fiducials associated to a specific
            // slice.
            sight::data::fiducials_series::referenced_image ri;
            /// ReferencedSOPClassUID (0008,1150)
            ri.referenced_sop_class_uid = image_series->get_sop_class_uid();
            /// ReferencedSOPInstanceUID (0008,1155)
            ri.referenced_sop_instance_uid = image_series->get_sop_instance_uid();

            const int slice_index = static_cast<int>(sight::data::helper::medical_image::get_slice_index(
                                                         *image_series,
                                                         sight::data::helper::medical_image::orientation_t::axial
            ).value_or(-1));

            /// ReferencedFrameNumber (0008,1160)
            ri.referenced_frame_number.push_back(static_cast<std::int32_t>(slice_index));
            /// ReferencedSegmentNumber (0062,000B)
            ri.referenced_segment_number.push_back(0);

            fiducial_set.referenced_image_sequence = std::vector<sight::data::fiducials_series::referenced_image>();
            fiducial_set.referenced_image_sequence->push_back(ri);
            fiducial_set.color = _color;

            image_series->get_fiducials()->append_fiducial_set(fiducial_set);
        }
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void ruler::create_ruler_ogre_set(
    const std::array<float, 4> _color,
    const float _sphere_radius,
    const std::optional<std::string> _id,
    const std::array<double, 3> _begin,
    const std::array<double, 3> _end,
    const bool _visible
)
{
    auto* const root_node = this->get_scene_manager()->getRootSceneNode();
    const auto color      = Ogre::ColourValue(_color[0], _color[1], _color[2], _color[3]);

    // In case the z coord is an integer, we should to set it a tiny bit behind.
    // Otherwise the ogre line will flicker.
    const auto begin_z_coord = std::trunc(_begin[2]) == _begin[2] ? _begin[2] - 0.001 : _begin[2];
    const auto end_z_coord   = std::trunc(_end[2]) == _end[2] ? _end[2] - 0.001 : _end[2];

    const Ogre::Vector3 v1(
        static_cast<float>(_begin[0]),
        static_cast<float>(_begin[1]),
        static_cast<float>(begin_z_coord)
    );

    const Ogre::Vector3 v2(
        static_cast<float>(_end[0]),
        static_cast<float>(_end[1]),
        static_cast<float>(end_z_coord)
    );

    const auto id =
        [&](const std::string& _type)
        {
            return this->get_id() + "_" + _type + "_"
                   + _id.value_or("");
        };

    // Create first sphere.
    auto* const sphere1 = this->get_scene_manager()->createManualObject(id("sphere1"));
    SIGHT_ASSERT("Can't create the first entity", sphere1);

    sight::viz::scene3d::helper::manual_object::create_sphere(
        sphere1,
        m_sphere_material_name,
        color,
        _sphere_radius
    );

    sphere1->setQueryFlags(m_query_flag);
    // Render this sphere over all others objects.
    sphere1->setRenderQueueGroup(RULER_RQ_GROUP_ID);

    Ogre::SceneNode* const node1 = root_node->createChildSceneNode(id("node1"), v1);
    SIGHT_ASSERT("Can't create the first node", node1);
    node1->attachObject(sphere1);

    // Create second sphere.
    auto* const sphere2 = this->get_scene_manager()->createManualObject(id("sphere2"));
    SIGHT_ASSERT("Can't create the second entity", sphere2);

    sight::viz::scene3d::helper::manual_object::create_sphere(
        sphere2,
        m_sphere_material_name,
        color,
        _sphere_radius
    );

    sphere2->setQueryFlags(m_query_flag);
    // Render this sphere over all others objects.
    sphere2->setRenderQueueGroup(RULER_RQ_GROUP_ID);

    Ogre::SceneNode* const node2 = root_node->createChildSceneNode(id("node2"), v2);
    SIGHT_ASSERT("Can't create the second node", node2);
    node2->attachObject(sphere2);

    // Line.
    auto* const line = this->get_scene_manager()->createManualObject(id("line"));
    SIGHT_ASSERT("Can't create the line", line);
    line->begin(
        m_line_material_name,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    line->colour(color);
    line->position(v1);
    line->position(v2);
    line->end();
    line->setQueryFlags(0x0);
    root_node->attachObject(line);

    // Dashed line.
    auto* const dashed_line = this->get_scene_manager()->createManualObject(id("dashed_line"));
    SIGHT_ASSERT("Can't create the dashed line", dashed_line);
    dashed_line->begin(
        m_dashed_line_material_name,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    dashed_line->colour(color);
    dashed_line->position(v1);
    sight::viz::scene3d::helper::manual_object::draw_dashed_line(
        dashed_line,
        v1,
        v2,
        _sphere_radius,
        _sphere_radius
    );
    dashed_line->end();
    dashed_line->setQueryFlags(0x0);
    // Render this line over all others objects.
    dashed_line->setRenderQueueGroup(RULER_RQ_GROUP_ID);
    root_node->attachObject(dashed_line);

    // Label.
    sight::viz::scene3d::text::sptr label = sight::viz::scene3d::text::make(this->layer());

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = sight::viz::scene3d::helper::scene::get_length(v1, v2);
    label->set_text(length);
    label->set_text_color(color);
    label->set_font_size(m_font_size);
    Ogre::SceneNode* const label_node = root_node->createChildSceneNode(id("label_node"), v2);
    SIGHT_ASSERT("Can't create the label node", label_node);
    label->attach_to_node(label_node, this->layer()->get_default_camera());

    // Set the visibility.
    sphere1->setVisible(_visible);
    sphere2->setVisible(_visible);
    line->setVisible(_visible);
    dashed_line->setVisible(_visible);
    label->set_visible(_visible);
    ruler::ruler_ogre_set created_ruler {_id, node1, sphere1, node2, sphere2, line, dashed_line, label_node, label};

    m_ruler_ogre_sets.emplace_back(created_ruler);
}

//------------------------------------------------------------------------------

void ruler::activate_tool(const bool _activate)
{
    if(_activate)
    {
        m_tool_activated = true;
        m_picked_ruler   = {nullptr, true};
        m_interactive    = true;

        set_cursor(Qt::CrossCursor);
        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        parent_widget->installEventFilter(m_event_filter.get());
    }
    else
    {
        m_tool_activated = false;
        m_creation_mode  = false;
        m_picked_ruler   = {nullptr, true};
        m_interactive    = false;

        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        parent_widget->unsetCursor();

        if(m_bin_button != nullptr)
        {
            m_bin_button->hide();
            delete m_bin_button;
            m_bin_button = nullptr;
        }
    }

    // We need to redraw the rulers because the update of m_interactive affects the size of the rulers.
    this->updating();
}

//------------------------------------------------------------------------------

void ruler::set_visible(bool _visible)
{
    m_visible = _visible;
    if(!m_tool_activated)
    {
        this->activate_tool(false);
    }
    else if(m_visible)
    {
        this->activate_tool(true);
    }
    else
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void ruler::remove_ruler_ogre_set(std::optional<std::string> _id)
{
    if(auto iter = std::ranges::find(m_ruler_ogre_sets, _id, &ruler_ogre_set::id); iter != m_ruler_ogre_sets.end())
    {
        iter->label->detach_from_node();
        iter->label->set_visible(false);
        iter->label.reset();
        this->get_scene_manager()->destroySceneNode(iter->node1);
        this->get_scene_manager()->destroyManualObject(iter->sphere1);
        this->get_scene_manager()->destroySceneNode(iter->node2);
        this->get_scene_manager()->destroyManualObject(iter->sphere2);
        this->get_scene_manager()->destroyManualObject(iter->line);
        this->get_scene_manager()->destroyManualObject(iter->dashed_line);
        this->get_scene_manager()->destroySceneNode(iter->label_node);

        m_ruler_ogre_sets.erase(iter);
    }
}

//------------------------------------------------------------------------------

void ruler::remove_ruler_fiducial(std::optional<std::string> _id)
{
    const auto image = m_image.lock();
    if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        std::vector<data::fiducials_series::fiducial_set> fiducial_sets =
            image_series->get_fiducials()->get_fiducial_sets();
        for(auto it_fiducial_set = fiducial_sets.begin() ; it_fiducial_set != fiducial_sets.end() ; )
        {
            for(auto it_fiducial = it_fiducial_set->fiducial_sequence.begin() ;
                it_fiducial != it_fiducial_set->fiducial_sequence.end() ; )
            {
                if(it_fiducial->shape_type == data::fiducials_series::shape::ruler
                   && it_fiducial->fiducial_uid == _id)
                {
                    it_fiducial = it_fiducial_set->fiducial_sequence.erase(it_fiducial);
                }
                else
                {
                    ++it_fiducial;
                }
            }

            if(it_fiducial_set->fiducial_sequence.empty())
            {
                it_fiducial_set = fiducial_sets.erase(it_fiducial_set);
            }
            else
            {
                ++it_fiducial_set;
            }
        }

        image_series->get_fiducials()->set_fiducial_sets(fiducial_sets);
    }

    const auto sig = image->signal<data::image::fiducial_removed_signal_t>(
        data::image::FIDUCIAL_REMOVED_SIG
    );
    sig->async_emit(_id);
}

//------------------------------------------------------------------------------

void ruler::remove_all()
{
    for(const auto& ruler : m_ruler_ogre_sets)
    {
        remove_ruler_fiducial(ruler.id);
    }
}

//------------------------------------------------------------------------------

void ruler::remove_from_current_slice()
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

    this->render_service()->make_current();

    for(auto& ruler : m_ruler_ogre_sets)
    {
        const std::array<double, 3> begin = {ruler.node1->getPosition().x, ruler.node1->getPosition().y,
                                             ruler.node1->getPosition().z
        };
        const std::array<double, 3> end = {ruler.node2->getPosition().x, ruler.node2->getPosition().y,
                                           ruler.node2->getPosition().z
        };

        if(this->is_visible_on_current_slice(begin, end))
        {
            this->remove_ruler_fiducial(ruler.id);
        }
    }
}

//------------------------------------------------------------------------------

void ruler::display_on_current_slice()
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

    if(!m_always_display_all)
    {
        this->render_service()->make_current();

        for(auto& ruler : m_ruler_ogre_sets)
        {
            const std::array<double, 3> begin = {ruler.node1->getPosition().x, ruler.node1->getPosition().y,
                                                 ruler.node1->getPosition().z
            };
            const std::array<double, 3> end = {ruler.node2->getPosition().x, ruler.node2->getPosition().y,
                                               ruler.node2->getPosition().z
            };
            bool visible_on_current_slice = this->is_visible_on_current_slice(begin, end);

            ruler.sphere1->setVisible(m_visible && visible_on_current_slice);
            ruler.sphere2->setVisible(m_visible && visible_on_current_slice);
            ruler.line->setVisible(m_visible && visible_on_current_slice);
            ruler.label->set_visible(m_visible && visible_on_current_slice);
            ruler.dashed_line->setVisible(m_visible && visible_on_current_slice);
        }
    }
}

//------------------------------------------------------------------------------

void ruler::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

    m_is_a_mouse_move = false;

    if(_button == left && m_tool_activated && m_visible && !m_creation_mode)
    {
        const sight::viz::scene3d::layer::csptr layer = this->layer();
        Ogre::SceneManager* const scene_mgr           = layer->get_scene_manager();

        const Ogre::Camera* const cam = layer->get_default_camera();
        const auto* const vp          = cam->getViewport();

        const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

        bool found                                 = false;
        Ogre::RaySceneQuery* const ray_scene_query = scene_mgr->createRayQuery(ray, m_query_flag);
        ray_scene_query->setSortByDistance(true);
        if(!ray_scene_query->execute().empty())
        {
            const Ogre::RaySceneQueryResult& query_result_vect = ray_scene_query->getLastResults();

            // First iterate over the ManualObjects to try to find an already existing matching point.
            for(std::size_t qr_idx = 0 ; qr_idx < query_result_vect.size() && !found ; qr_idx++)
            {
                const Ogre::MovableObject* const object = query_result_vect[qr_idx].movable;
                const auto object_type                  = object->getMovableType();

                if(object_type == "ManualObject" && object->isVisible())
                {
                    const Ogre::Real scale = 1.15F;

                    for(auto& ruler : m_ruler_ogre_sets)
                    {
                        if(ruler.sphere1 == object)
                        {
                            ruler.node1->setScale(scale, scale, scale);
                            m_picked_ruler = {&ruler, true};
                            found          = true;

                            this->set_cursor(Qt::ClosedHandCursor);
                        }

                        if(ruler.sphere2 == object)
                        {
                            ruler.node2->setScale(scale, scale, scale);
                            m_picked_ruler = {&ruler, false};
                            found          = true;

                            this->set_cursor(Qt::ClosedHandCursor);
                        }
                    }
                }
            }

            // Otherwise, we might have to create a new ruler.
            if(!found)
            {
                for(std::size_t qr_idx = 0 ; qr_idx < query_result_vect.size() && !found ; qr_idx++)
                {
                    const Ogre::MovableObject* const object = query_result_vect[qr_idx].movable;
                    const auto object_type                  = object->getMovableType();

                    if(object_type == "Entity" && object->isVisible())
                    {
                        // First point
                        const auto pick_result = sight::viz::scene3d::utils::pick_object(
                            _x,
                            _y,
                            m_query_mask,
                            *scene_mgr,
                            true
                        );

                        if(pick_result.has_value())
                        {
                            const auto clicked_position = pick_result->second;
                            const std::array<double,
                                             3> position = {clicked_position.x, clicked_position.y, clicked_position.z
                            };
                            const auto ruler_fiducial_uid          = sight::core::tools::uuid::generate();
                            const Ogre::ColourValue& default_color = this->get_default_color();

                            this->create_ruler_fiducial(
                                {default_color[0], default_color[1], default_color[2], default_color[3]},
                                ruler_fiducial_uid,
                                position,
                                position
                            );

                            this->create_ruler_ogre_set(
                                {default_color[0], default_color[1], default_color[2], default_color[3]},
                                m_sphere_radius * (m_interactive ? 2.0F : 1.0F),
                                ruler_fiducial_uid,
                                position,
                                position,
                                m_visible
                            );
                            m_picked_ruler = {&m_ruler_ogre_sets[m_ruler_ogre_sets.size() - 1], true};

                            // Remembers that we enter in a mode where we need to place the second point.
                            m_creation_mode = true;

                            this->set_cursor(Qt::ClosedHandCursor);

                            break;
                        }
                    }
                }
            }
        }

        scene_mgr->destroyQuery(ray_scene_query);
    }
}

//------------------------------------------------------------------------------

void ruler::mouse_move_event(
    mouse_button /*_button*/,
    modifier /*_mods*/,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    m_is_a_mouse_move = true;

    if(m_tool_activated && m_visible)
    {
        this->render_service()->make_current();

        if(m_picked_ruler.m_data != nullptr)
        {
            // Discard the current ruler to launch the ray over the scene without picking this one.
            m_picked_ruler.m_data->sphere1->setQueryFlags(0x0);
            m_picked_ruler.m_data->sphere2->setQueryFlags(0x0);

            const auto layer              = this->layer();
            const Ogre::Camera* const cam = layer->get_default_camera();
            SIGHT_ASSERT("No camera found", cam);

            bool move_in_camera_plane = true;

            Ogre::Vector3 new_pos;
            if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
            {
                Ogre::SceneManager* const scene_mgr = layer->get_scene_manager();
                // If something is picked, we will snap data to it.
                auto picked_pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr, true);
                if(picked_pos.has_value())
                {
                    new_pos              = picked_pos->second;
                    move_in_camera_plane = false;
                }
            }

            // Else we move the ruler along a plane.
            if(move_in_camera_plane)
            {
                const auto* const vp = cam->getViewport();

                const float vp_x = static_cast<float>(_x - vp->getActualLeft())
                                   / static_cast<float>(vp->getActualWidth());
                const float vp_y = static_cast<float>(_y - vp->getActualTop())
                                   / static_cast<float>(vp->getActualHeight());

                const Ogre::Ray ray           = cam->getCameraToViewportRay(vp_x, vp_y);
                const Ogre::Vector3 direction = sight::viz::scene3d::helper::camera::get_cam_direction(cam);

                Ogre::Vector3 position;
                // Check if we already place the first point.
                if(m_picked_ruler.m_first)
                {
                    position = m_picked_ruler.m_data->node1->getPosition();
                }
                else
                {
                    position = m_picked_ruler.m_data->node2->getPosition();
                }

                const Ogre::Plane plane(direction, position);

                const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

                if(!hit.first)
                {
                    SIGHT_ERROR("The ray must intersect the plane");
                    return;
                }

                new_pos = ray.getPoint(hit.second);
            }

            // Reset the query flag.
            m_picked_ruler.m_data->sphere1->setQueryFlags(m_query_flag);
            m_picked_ruler.m_data->sphere2->setQueryFlags(m_query_flag);

            if(m_picked_ruler.m_first)
            {
                this->update_picked_ruler(
                    m_picked_ruler.m_data,
                    new_pos,
                    m_picked_ruler.m_data->node2->getPosition()
                );
            }
            else
            {
                this->update_picked_ruler(
                    m_picked_ruler.m_data,
                    m_picked_ruler.m_data->node1->getPosition(),
                    new_pos
                );
            }

            this->request_render();

            const auto image = m_image.lock();
            const auto& sig  = image->signal<sight::data::image_series::ruler_modified_signal_t>(
                sight::data::image_series::RULER_MODIFIED_SIG
            );
            sig->async_emit(
                m_picked_ruler.m_data->id.value_or(
                    ""
                ),
                {m_picked_ruler.m_data->node1->getPosition().x,
                 m_picked_ruler.m_data->node1->getPosition().y, m_picked_ruler.m_data->node1->getPosition().z
                },
                {m_picked_ruler.m_data->node2->getPosition().x,
                 m_picked_ruler.m_data->node2->getPosition().y, m_picked_ruler.m_data->node2->getPosition().z
                });
        }
        else
        {
            const sight::viz::scene3d::layer::csptr layer = this->layer();
            Ogre::SceneManager* const scene_mgr           = layer->get_scene_manager();

            const Ogre::Camera* const cam = layer->get_default_camera();
            const auto* const vp          = cam->getViewport();

            const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

            bool found                                 = false;
            Ogre::RaySceneQuery* const ray_scene_query = scene_mgr->createRayQuery(ray, m_query_flag);
            ray_scene_query->setSortByDistance(true);
            if(!ray_scene_query->execute().empty())
            {
                const Ogre::RaySceneQueryResult& query_result_vect = ray_scene_query->getLastResults();

                for(std::size_t qr_idx = 0 ; qr_idx < query_result_vect.size() && !found ; qr_idx++)
                {
                    const Ogre::MovableObject* const object = query_result_vect[qr_idx].movable;
                    const auto object_type                  = object->getMovableType();

                    if(object_type == "ManualObject" && object->isVisible())
                    {
                        for(auto& ruler : m_ruler_ogre_sets)
                        {
                            if(ruler.sphere1 == object || ruler.sphere2 == object)
                            {
                                this->set_cursor(Qt::OpenHandCursor);
                                m_is_over_ruler = true;
                                found           = true;
                                break;
                            }
                        }
                    }
                }

                if(m_is_over_ruler && !found)
                {
                    m_is_over_ruler = false;
                    this->set_cursor(Qt::CrossCursor);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ruler::button_release_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(_button == left && m_picked_ruler.m_data != nullptr)
    {
        const Ogre::Real scale = 1.F;
        m_picked_ruler.m_data->node1->setScale(scale, scale, scale);
        m_picked_ruler.m_data->node2->setScale(scale, scale, scale);

        const int length =
            static_cast<int>(std::round(
                                 (m_picked_ruler.m_data->node2->getPosition()
                                  - m_picked_ruler.m_data->node1->getPosition()).length()
            ));

        // If it is in creation mode, and a distance is calculated, it is the second point of the ruler.
        // Finish the ruler creation.
        if(m_creation_mode && length != 0)
        {
            m_creation_mode = false;
            this->set_cursor(Qt::OpenHandCursor);

            m_picked_ruler = {nullptr, true};
        }
        // If it is not a creation mode, a distance null means that the ruler should be removed.
        else if(!m_creation_mode)
        {
            if(length == 0)
            {
                this->set_cursor(Qt::CrossCursor);
                this->remove_ruler_fiducial(m_picked_ruler.m_data->id);
                m_picked_ruler = {nullptr, true};
            }
            else
            {
                if(!m_is_a_mouse_move)
                {
                    auto interactor     = layer()->render_service()->get_interactor_manager();
                    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
                    auto* parent_widget = qt_interactor->get_qt_widget();
                    static const QIcon s_TRASH_BIN_ICON(QString::fromStdString(
                                                            (core::runtime::get_module_resource_path(
                                                                 "sight::module::ui::icons"
                                                             ) / "trash.svg").string()
                    ));
                    m_bin_button = new QPushButton(s_TRASH_BIN_ICON, "", parent_widget);
                    const std::string service_id = base_id();
                    m_bin_button->setObjectName(QString::fromStdString(service_id) + "/binButton");
                    m_bin_button->setCursor(Qt::ArrowCursor);
                    m_bin_button->adjustSize();
                    if(m_bin_button->width() < 40)
                    {
                        m_bin_button->setMinimumWidth(40);
                    }

                    if(m_bin_button->height() < 40)
                    {
                        m_bin_button->setMinimumHeight(40);
                    }

                    m_bin_button->adjustSize();
                    m_bin_button->setIconSize(m_bin_button->size());
                    m_bin_button->raise();
                    Ogre::SceneNode* node =
                        m_picked_ruler.m_first ? m_picked_ruler.m_data->node1 : m_picked_ruler.m_data->node2;
                    std::pair<Ogre::Vector2,
                              Ogre::Vector2> screen_pos = sight::viz::scene3d::helper::scene::compute_bounding_rect(
                        *layer()->get_default_camera(),
                        node
                              );
                    double ratio = m_bin_button->devicePixelRatioF();
                    const int x  = std::clamp(
                        int(((screen_pos.first.x + screen_pos.second.x) / 2) / ratio),
                        0,
                        parent_widget->width() - m_bin_button->width()
                    );
                    int y = int((screen_pos.first.y / ratio) - m_bin_button->height());
                    if(y < 0)
                    {
                        // If there isn't enough place upward the landmark, place the menu downward.
                        y = int(screen_pos.second.y / ratio);
                    }

                    m_bin_button->move(x, y);
                    m_bin_button->show();
                    QObject::connect(
                        m_bin_button,
                        &QPushButton::clicked,
                        [this, id = m_picked_ruler.m_data->id]
                        {
                            m_bin_button->hide();
                            this->remove_ruler_fiducial(id);
                            this->set_cursor(Qt::CrossCursor);
                            m_picked_ruler = {nullptr, true};

                            delete m_bin_button;
                            m_bin_button = nullptr;
                        });
                }

                this->set_cursor(Qt::OpenHandCursor);
                m_picked_ruler = {nullptr, true};
            }
        }

        this->layer()->request_render();
    }
    else if(_button == right)
    {
        // Right button in creation mode destroys the created ruler.
        if(m_creation_mode)
        {
            m_creation_mode = false;
            this->set_cursor(Qt::CrossCursor);
            const auto id = m_picked_ruler.m_data->id;
            this->remove_ruler_fiducial(id);
            m_picked_ruler = {nullptr, true};
        }
        // Right button other than in creation mode goes out of the add ruler mode.
        else
        {
            this->activate_tool(false);
            this->signal<signals::void_signal_t>(signals::TOOL_DEACTIVATED)->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void ruler::update_picked_ruler(ruler_ogre_set* _ruler_to_update, Ogre::Vector3 _begin, Ogre::Vector3 _end)
{
    SIGHT_ASSERT("Ogre ruler elements can't be null", _ruler_to_update);

    _ruler_to_update->node1->setPosition(_begin);
    _ruler_to_update->node2->setPosition(_end);

    Ogre::ManualObject* const line = _ruler_to_update->line;
    line->beginUpdate(0);
    line->position(_begin);
    line->position(_end);
    line->end();

    Ogre::ManualObject* const dashed_line = _ruler_to_update->dashed_line;
    dashed_line->beginUpdate(0);
    sight::viz::scene3d::helper::manual_object::draw_dashed_line(
        dashed_line,
        _begin,
        _end,
        m_sphere_radius * (m_interactive ? 2.0F : 1.0F),
        m_sphere_radius * (m_interactive ? 2.0F : 1.0F)
    );
    dashed_line->end();

    const std::string length = sight::viz::scene3d::helper::scene::get_length(_begin, _end);
    _ruler_to_update->label->set_text(length);
    _ruler_to_update->label_node->setPosition(_end);

    {
        const auto image = m_image.lock();
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            // With fields, the modified point list is a shared pointer to the ruler field, so there is
            // nothing more
            // to do; however this isn't enough for fiducials, which must be updated manually.
            std::vector<data::fiducials_series::fiducial_set> fiducial_sets =
                image_series->get_fiducials()->get_fiducial_sets();
            for(data::fiducials_series::fiducial_set& fiducial_set : fiducial_sets)
            {
                for(data::fiducials_series::fiducial& fiducial : fiducial_set.fiducial_sequence)
                {
                    if(fiducial.shape_type == data::fiducials_series::shape::ruler
                       && fiducial.fiducial_uid == _ruler_to_update->id)
                    {
                        fiducial.contour_data.clear();
                        fiducial.contour_data.push_back({.x = _begin.x, .y = _begin.y, .z = _begin.z});
                        fiducial.contour_data.push_back({.x = _end.x, .y = _end.y, .z = _end.z});
                    }
                }
            }

            image_series->get_fiducials()->set_fiducial_sets(fiducial_sets);
        }
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void ruler::update_modified_ruler(
    std::optional<std::string> _id,
    std::array<double, 3> _begin,
    std::array<double, 3> _end
)
{
    if(auto iter = std::ranges::find(m_ruler_ogre_sets, _id, &ruler_ogre_set::id); iter == m_ruler_ogre_sets.end())
    {
        // Create ruler if it doesn't exist.
        const Ogre::ColourValue& default_color = this->get_default_color();

        this->create_ruler_ogre_set(
            {default_color[0], default_color[1], default_color[2], default_color[3]},
            m_sphere_radius * (m_interactive ? 2.0F : 1.0F),
            _id,
            _begin,
            _end,
            m_visible
        );
        this->display_on_current_slice();
    }
    else
    {
        // If it already exists, update ruler with the new position.
        const Ogre::Vector3 begin = Ogre::Vector3(
            static_cast<float>(_begin[0]),
            static_cast<float>(_begin[1]),
            static_cast<float>(_begin[2])
        );
        const Ogre::Vector3 end = Ogre::Vector3(
            static_cast<float>(_end[0]),
            static_cast<float>(_end[1]),
            static_cast<float>(_end[2])
        );

        this->update_picked_ruler(&(*iter), begin, end);
        this->display_on_current_slice();
    }
}

//------------------------------------------------------------------------------

void ruler::key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_tool_activated && _key == Qt::Key_Escape)
    {
        this->activate_tool(false);
        this->signal<signals::void_signal_t>(signals::TOOL_DEACTIVATED)->async_emit();
    }
}

//------------------------------------------------------------------------------

void ruler::set_cursor(QCursor _cursor)
{
    auto interactor     = layer()->render_service()->get_interactor_manager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->get_qt_widget();
    parent_widget->setCursor(_cursor);
}

//------------------------------------------------------------------------------

Ogre::ColourValue ruler::get_default_color()
{
    if(m_color.empty())
    {
        return sight::viz::scene3d::helper::scene::generate_color(m_color_index++);
    }

    sight::ui::color_t color;
    sight::data::tools::color::hexa_string_to_rgba(m_color, color);

    return Ogre::ColourValue(color.data());
}

//------------------------------------------------------------------------------

bool ruler::is_visible_on_current_slice(std::array<double, 3> _begin, std::array<double, 3> _end)
{
    bool visible               = false;
    const auto& slice_position =
        [this]
        {
            const auto image_series = m_image.const_lock();
            const auto& spacing     = image_series->spacing();
            const auto& origin      = image_series->origin();
            const auto slice_index  = sight::data::helper::medical_image::get_slice_index(
                *image_series,
                m_orientation
            ).value_or(0);

            return origin[m_orientation] + (double(slice_index) * spacing[m_orientation]);
        }();

    const auto position1 =
        m_orientation == orientation_t::z_axis
        ? _begin[2]
        : m_orientation == orientation_t::y_axis
        ? _begin[1]
        : _begin[0];

    const auto position2 =
        m_orientation == orientation_t::z_axis
        ? _end[2]
        : m_orientation == orientation_t::y_axis
        ? _end[1]
        : _end[0];

    const auto rounded_position1      = std::round(position1);
    const auto rounded_position2      = std::round(position2);
    const auto rounded_slice_position = std::round(slice_position);

    // Check if the ruler positions are on the current slice or in between.
    if((rounded_slice_position - rounded_position1) * (rounded_slice_position - rounded_position2) <= 0)
    {
        visible = true;
    }

    return visible;
}

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials.
