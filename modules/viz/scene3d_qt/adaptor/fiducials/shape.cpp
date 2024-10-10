/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "shape.hpp"

#include "core/runtime/path.hpp"

#include "data/fiducials_series.hpp"
#include "data/helper/fiducials_series.hpp"

#include <core/com/slots.hxx>
#include <core/tools/uuid.hpp>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/point_list.hpp>
#include <data/tools/color.hpp>

#include <io/opencv/image.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

#include <OgreEntity.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

static constexpr std::uint8_t SHAPE_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

//------------------------------------------------------------------------------

shape::shape() noexcept
{
    new_slot(slots::REMOVE_ALL, &shape::remove_all, this);
    new_slot(slots::REMOVE_SHAPES, &shape::remove_shapes, this);
    new_slot(slots::ACTIVATE_SHAPE_TOOL, &shape::activate_shape_tool, this);
    new_slot(slots::UPDATE_MODIFIED_SHAPE, &shape::update_modified_shape, this);
    new_slot(slots::RESTRICT_TO_CURRENT_SLICE, &shape::restrict_to_current_slice, this);

    new_signal<signals::void_signal_t>(signals::DEACTIVATE_SHAPE_TOOL);
}

//------------------------------------------------------------------------------

service::connections_t shape::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void shape::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_RADIUS_CONFIG      = CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = CONFIG + "queryFlags";
    static const std::string s_COLOR_CONFIG       = CONFIG + "color";

    m_font_size     = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_sphere_radius = config.get<float>(s_RADIUS_CONFIG, m_sphere_radius);
    m_interactive   = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority      = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_config_color  = config.get<std::string>(s_COLOR_CONFIG, m_config_color);

    std::string hexa_mask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_mask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    hexa_mask = config.get<std::string>(s_QUERY_FLAGS_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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
    m_sphere_material = std::make_unique<sight::viz::scene3d::material>(
        m_sphere_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_sphere_material->set_has_vertex_color(true);
    m_sphere_material->update_shading_mode(data::material::shading_t::phong, layer->num_lights(), false, false);

    m_line_material = std::make_unique<sight::viz::scene3d::material>(
        m_line_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_line_material->set_has_vertex_color(true);
    m_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);

    m_dashed_line_material = std::make_unique<sight::viz::scene3d::material>(
        m_dashed_line_material_name,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_dashed_line_material->set_has_vertex_color(true);
    m_dashed_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);

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

    this->update_from_fiducials();
}

//------------------------------------------------------------------------------

void shape::updating()
{
    this->render_service()->make_current();

    const sight::viz::scene3d::layer::csptr layer = this->layer();

    m_sphere_material->update_shading_mode(data::material::shading_t::phong, layer->num_lights(), false, false);
    m_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
    m_dashed_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
}

//------------------------------------------------------------------------------

void shape::update_from_fiducials()
{
    while(!m_shapes.empty())
    {
        this->destroy_shape(m_shapes.begin()->first);
    }

    m_cached_fiducials.clear();

    // First collect the point lists to be added (to prevent deadlocks on function calls)
    {
        const auto image = m_image.lock();

        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            auto fiducials = image_series->get_fiducials()->filter_fiducials(
                data::fiducials_series::shape::shape
            );

            std::copy(fiducials.begin(), fiducials.end(), std::back_inserter(m_cached_fiducials));
        }
    }

    for(const auto& fiducial : m_cached_fiducials)
    {
        std::string id = fiducial.fiducial_identifier;
        if( /* Check that the distance hasn't been already inserted */
            m_shapes.find(id) == m_shapes.end())
        {
            shape_data data;
            m_shapes[id] = data;

            auto pl = sight::data::helper::fiducials_series::to_point_list(fiducial);
            pl->set_id(id);

            m_shapes[id].m_point_list = pl;
        }
    }
}

//------------------------------------------------------------------------------

void shape::restrict_to_current_slice()
{
    int slice_index = -1;

    std::vector<data::fiducials_series::fiducial> fiducials;
    {
        const auto image = m_image.lock();
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            slice_index = static_cast<int>(sight::data::helper::medical_image::get_slice_index(
                                               *image_series,
                                               sight::data::helper::medical_image::orientation_t::axial
            ).value_or(-1));
        }

        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            fiducials = image_series->get_fiducials()->filter_fiducials(
                data::fiducials_series::shape::shape,
                slice_index
            );
        }
    }

    if(slice_index < 0)
    {
        SIGHT_ERROR("Invalid slice index. Cannot extract inferred data.");
        return;
    }

    // Hide everything
    for(const auto& cached_fiducial : m_cached_fiducials)
    {
        std::string id = cached_fiducial.fiducial_identifier;
        this->create_mask(nullptr);
        this->set_visible(id, false);
    }

    // Only show the current fiducials
    for(const auto& fiducial : fiducials)
    {
        std::string id = fiducial.fiducial_identifier;

        if(m_tool_activated)
        {
            this->create_shape(m_shapes[id].m_point_list);
            this->set_visible(id, true);
            this->create_mask(nullptr);
        }
        else
        {
            this->set_visible(id, false);
            this->create_mask(m_shapes[id].m_point_list);
        }
    }
}

//------------------------------------------------------------------------------

void shape::stopping()
{
    this->render_service()->make_current();

    m_sphere_material.reset();
    m_line_material.reset();
    m_dashed_line_material.reset();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->remove_interactor(interactor);
    }

    while(!m_shapes.empty())
    {
        this->destroy_shape(m_shapes.begin()->first);
    }

    m_eventFilter = nullptr;

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void shape::remove_all()
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

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
                if(it_fiducial->shape_type == data::fiducials_series::shape::shape)
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
}

//------------------------------------------------------------------------------

void shape::remove_shapes()
{
    this->render_service()->make_current();

    const auto image = m_image.lock();

    data::vector::sptr shape_vector;
    if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        shape_vector = std::make_shared<data::vector>();
        for(const data::fiducials_series::fiducial_set& fiducial_set :
            image_series->get_fiducials()->get_fiducial_sets())
        {
            for(const data::fiducials_series::fiducial& fiducial : fiducial_set.fiducial_sequence)
            {
                if(fiducial.shape_type == data::fiducials_series::shape::shape)
                {
                    if(data::point_list::sptr pl = sight::data::helper::fiducials_series::to_point_list(fiducial))
                    {
                        shape_vector->push_back(pl);
                    }
                }
            }
        }
    }

    std::vector<core::id::type> found_id;
    if(shape_vector)
    {
        for(const auto& object : *shape_vector)
        {
            found_id.push_back(object->get_id());
        }
    }
    else
    {
        while(!m_shapes.empty())
        {
            this->destroy_shape(m_shapes.begin()->first);
        }
    }

    std::vector<core::id::type> current_id;
    for(const auto& [id, _] : m_shapes)
    {
        current_id.push_back(id);
    }

    for(const core::id::type& id : current_id)
    {
        if(std::find(found_id.begin(), found_id.end(), id) == found_id.end())
        {
            destroy_shape(id);
        }
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void shape::set_visible(std::string _id, bool _visible)
{
    this->render_service()->make_current();

    auto it_shape = m_shapes.find(_id);

    if(it_shape != m_shapes.end())
    {
        const shape_data& data = it_shape->second;

        for(std::size_t i = 0 ; i < data.m_spheres.size() ; i++)
        {
            data.m_spheres.at(i)->setVisible(_visible);
            data.m_lines.at(i)->setVisible(_visible);
            data.m_dashed_lines.at(i)->setVisible(_visible);
        }
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void shape::set_visible(bool _visible)
{
    this->render_service()->make_current();

    for(const auto& shape : m_shapes)
    {
        const shape_data& data = shape.second;

        for(std::size_t i = 0 ; i < data.m_spheres.size() ; i++)
        {
            data.m_spheres.at(i)->setVisible(_visible);
            data.m_lines.at(i)->setVisible(_visible);
            data.m_dashed_lines.at(i)->setVisible(_visible);
        }
    }

    m_visible = _visible;

    this->request_render();
}

//------------------------------------------------------------------------------

void shape::button_press_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::mouse_move_event(
    mouse_button /*_button*/,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int /*_dx*/,
    int /*_dy*/
)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::key_press_event(int /*_key*/, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::create_mask(data::point_list::sptr _pl)
{
    const auto image = m_image.const_lock();
    auto mask        = m_mask.lock();

    if(!image)
    {
        SIGHT_ERROR("Invalid image given when creating ellipse.");
        return;
    }

    if(!mask)
    {
        SIGHT_ERROR("Invalid mask given when creating ellipse.");
        return;
    }

    // Check if image dimensions has changed
    if(mask->size()[0] != image->size()[0] || mask->size()[1] != image->size()[1])
    {
        auto size = image->size();
        // Has to be set to 0 for the moveToCv to work
        size[2] = 0;
        mask->resize(size, image->type(), sight::data::image::pixel_format::rgba);

        mask->set_origin(image->origin());
        mask->set_spacing(image->spacing());
        mask->set_window_width({1.});
        mask->set_window_center({0.});
    }

    // Set the mask to 0
    std::memset(mask->buffer(), 0, mask->size_in_bytes());

    // If the input point list is empty we forward the currently empty mask
    if(_pl == nullptr)
    {
        // Emit the image output signal
        auto sig = mask->signal<sight::data::image::buffer_modified_signal_t>(
            sight::data::image::BUFFER_MODIFIED_SIG
        );
        sig->async_emit();

        return;
    }

    try
    {
        std::vector<cv::Point> contour;
        const auto& points     = _pl->get_points();
        const std::size_t size = points.size();

        if(size > 5)
        {
            contour.reserve(size);
            const auto& spacing = image->spacing();

            for(const auto& point : points)
            {
                auto coord = point->get_coord();
                coord[0] /= spacing[0];
                coord[1] /= spacing[1];

                contour.emplace_back(
                    static_cast<int>(std::nearbyint(coord[0])),
                    static_cast<int>(std::nearbyint(coord[1]))
                );
            }
        }

        std::optional<Ogre::ColourValue> colour = std::nullopt;
        if(!m_config_color.empty())
        {
            std::array<unsigned char, 4> color_array {};
            data::tools::color::hexa_string_to_rgba(m_config_color, color_array);
            colour = Ogre::ColourValue(
                static_cast<float>(color_array[0]) / 255.F,
                static_cast<float>(color_array[1]) / 255.F,
                static_cast<float>(color_array[2]) / 255.F
            );
        }

        // If no color has been assigned, we generate one
        if(!colour.has_value())
        {
            colour = sight::viz::scene3d::helper::scene::generate_color(m_color_index++);
        }

        if(!contour.empty())
        {
            cv::Mat output = sight::io::opencv::image::move_to_cv(mask.get_shared());

            // Bounding ellipse
            cv::RotatedRect min_ellipse = cv::fitEllipse(contour);
            cv::ellipse(
                output,
                min_ellipse,
                {
                    static_cast<double>(colour->r * 255.),
                    static_cast<double>(colour->g * 255.),
                    static_cast<double>(colour->b * 255.),
                    255.0
                },
                2
            );

            // Emit the image output signal
            auto sig = mask->signal<sight::data::image::buffer_modified_signal_t>(
                sight::data::image::BUFFER_MODIFIED_SIG
            );
            sig->async_emit();
        }
    }
    catch(cv::Exception& e)
    {
        SIGHT_ERROR("OpenCV Exception caught: " << e.what());
    }
}

//------------------------------------------------------------------------------

void shape::create_shape(data::point_list::sptr _pl)
{
    const core::id::type id = _pl->get_id();

    if(auto it = m_shapes.find(id); it != m_shapes.end() && !(it->second.m_nodes.empty()))
    {
        return;
    }

    this->render_service()->make_current();

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
    Ogre::SceneNode* const root_node    = scene_mgr->getRootSceneNode();

    std::optional<Ogre::ColourValue> colour = std::nullopt;
    if(!m_config_color.empty())
    {
        std::array<unsigned char, 4> color_array {};
        data::tools::color::hexa_string_to_rgba(m_config_color, color_array);
        colour = Ogre::ColourValue(
            static_cast<float>(color_array[0]) / 255.F,
            static_cast<float>(color_array[1]) / 255.F,
            static_cast<float>(color_array[2]) / 255.F
        );
    }

    // If no color has been assigned, we generate one
    if(!colour.has_value())
    {
        colour = sight::viz::scene3d::helper::scene::generate_color(m_color_index++);
    }

    if(m_shapes.find(id) == m_shapes.end())
    {
        shape_data data;
        m_shapes[id] = data;
    }

    m_shapes[id].m_point_list = _pl;

    // Iterate over point pairs
    // Only construct the sphere for the first point and the line to the following one
    // This will construct the whole contour
    // No post iteration is required as: p[0] === p[p.size() - 1]

    // We at least need 2 points
    if(_pl->get_points().size() > 1)
    {
        auto& points = _pl->get_points();
        for(std::size_t i = 0 ; i < points.size() - 1 ; i++)
        {
            const std::array<double, 3> p1 = points.at(i)->get_coord();
            const std::array<double, 3> p2 = points.at(i + 1)->get_coord();

            const Ogre::Vector3 v1 = Ogre::Vector3(
                static_cast<float>(p1[0]),
                static_cast<float>(p1[1]),
                static_cast<float>(p1[2])
            );
            const Ogre::Vector3 v2 = Ogre::Vector3(
                static_cast<float>(p2[0]),
                static_cast<float>(p2[1]),
                static_cast<float>(p2[2])
            );

            // First sphere.
            Ogre::ManualObject* const sphere1 = scene_mgr->createManualObject(
                this->get_id() + "_sphere" + std::to_string(
                    i
                ) + "_" + id
            );
            sight::viz::scene3d::helper::manual_object::create_sphere(
                sphere1,
                m_sphere_material_name,
                *colour,
                m_sphere_radius
            );

            sphere1->setQueryFlags(m_query_flag);
            // Render this sphere over all others objects.
            sphere1->setRenderQueueGroup(SHAPE_RQ_GROUP_ID);
            SIGHT_ASSERT("Can't create the first entity", sphere1);
            Ogre::SceneNode* const node1 = root_node->createChildSceneNode(
                this->get_id() + "_node" + std::to_string(
                    i
                ) + "_" + id,
                v1
            );
            SIGHT_ASSERT("Can't create the first node", node1);
            node1->attachObject(sphere1);

            // Line.
            Ogre::ManualObject* const line = scene_mgr->createManualObject(
                this->get_id() + "_line" + std::to_string(
                    i
                ) + "_" + id
            );
            SIGHT_ASSERT("Can't create the line", line);
            line->begin(m_line_material_name, Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
            line->colour(*colour);
            line->position(v1);
            line->position(v2);
            line->end();
            line->setQueryFlags(0x0);
            root_node->attachObject(line);

            // Dashed line.
            Ogre::ManualObject* const dashed_line = scene_mgr->createManualObject(
                this->get_id() + "_dashedLine" + std::to_string(i) + "_" + id
            );
            SIGHT_ASSERT("Can't create the dashed line", dashed_line);
            dashed_line->begin(
                m_dashed_line_material_name,
                Ogre::RenderOperation::OT_LINE_LIST,
                sight::viz::scene3d::RESOURCE_GROUP
            );
            dashed_line->colour(*colour);
            sight::viz::scene3d::helper::manual_object::draw_dashed_line(
                dashed_line,
                v1,
                v2,
                m_sphere_radius,
                m_sphere_radius,
                colour
            );
            dashed_line->end();
            dashed_line->setQueryFlags(0x0);
            // Render this line over all others objects.
            dashed_line->setRenderQueueGroup(SHAPE_RQ_GROUP_ID);
            root_node->attachObject(dashed_line);

            // Set the visibility.
            sphere1->setVisible(m_visible);
            line->setVisible(m_visible);
            dashed_line->setVisible(m_visible);

            m_shapes[id].m_nodes.push_back(node1);
            m_shapes[id].m_spheres.push_back(sphere1);
            m_shapes[id].m_lines.push_back(line);
            m_shapes[id].m_dashed_lines.push_back(dashed_line);
        }
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void shape::update_image_fiducials(data::point_list::sptr _pl)
{
    const auto image = m_image.lock();
    if(data::helper::medical_image::check_image_validity(image.get_shared()))
    {
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()); image_series)
        {
            data::fiducials_series::fiducial_set fiducial_set;
            std::string frame_of_reference_uid = image_series->get_string_value(
                data::dicom::attribute::Keyword::FrameOfReferenceUID
            );
            if(frame_of_reference_uid.empty())
            {
                // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory
                // according
                // to the DICOM standard anyway.
                frame_of_reference_uid = core::tools::uuid::generate();
                image_series->set_string_value(
                    data::dicom::attribute::Keyword::FrameOfReferenceUID,
                    frame_of_reference_uid
                );
            }

            fiducial_set.frame_of_reference_uid = frame_of_reference_uid;
            data::fiducials_series::fiducial fiducial;
            fiducial.shape_type           = data::fiducials_series::shape::shape;
            fiducial.fiducial_identifier  = _pl->get_id();
            fiducial.fiducial_description = "Shape";
            fiducial.fiducial_uid         = _pl->get_id();
            std::array<double, 3> first_point  = _pl->get_points().front()->get_coord();
            std::array<double, 3> second_point = _pl->get_points().back()->get_coord();
            fiducial.contour_data =
            {
                {.x = first_point[0], .y = first_point[1], .z = first_point[2]},
                {.x = second_point[0], .y = second_point[1], .z = second_point[2]}
            };
            // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too
            // lazy to do that, so I simply get rid of GraphicCoordinatesDataSequence.
            fiducial.graphic_coordinates_data_sequence = std::nullopt;
            fiducial_set.fiducial_sequence.push_back(fiducial);
            image_series->get_fiducials()->append_fiducial_set(fiducial_set);
        }
    }
}

//------------------------------------------------------------------------------

void shape::update_shape(
    const shape_data* const /*_data*/,
    Ogre::Vector3 /*_begin*/,
    Ogre::Vector3 /*_end*/
)
{
    /// No implemented interaction for editing shapes as of now
}

//------------------------------------------------------------------------------

void shape::destroy_shape(core::id::type _id)
{
    const auto it = m_shapes.find(_id);
    SIGHT_ASSERT("The shape is not found", it != m_shapes.end());

    // Destroy Ogre resource.
    shape_data shape                    = it->second;
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    for(auto& node : shape.m_nodes)
    {
        scene_mgr->destroySceneNode(node);
    }

    for(auto& sphere : shape.m_spheres)
    {
        scene_mgr->destroyManualObject(sphere);
    }

    for(auto& line : shape.m_lines)
    {
        scene_mgr->destroyManualObject(line);
    }

    for(auto& dashed_line : shape.m_dashed_lines)
    {
        scene_mgr->destroyManualObject(dashed_line);
    }

    // Remove it from the map.
    m_shapes.erase(it);
}

//------------------------------------------------------------------------------

void shape::activate_shape_tool(bool _activate)
{
    m_tool_activated = _activate;

    this->restrict_to_current_slice();
}

//------------------------------------------------------------------------------

void shape::update_modified_shape(data::point_list::sptr _pl)
{
    this->render_service()->make_current();

    if(m_shapes.find(_pl->get_id()) == m_shapes.end())
    {
        // create shape if it doesn't exist
        this->create_shape(_pl);
    }

    /// No implemented interaction for editing shapes as of now

    this->request_render();
}

//------------------------------------------------------------------------------

void shape::set_cursor(QCursor _cursor)
{
    auto interactor     = layer()->render_service()->get_interactor_manager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->get_qt_widget();
    parent_widget->setCursor(_cursor);
}

//------------------------------------------------------------------------------

void shape::remove_shape(data::point_list::sptr _pl)
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
                if(it_fiducial->shape_type == data::fiducials_series::shape::shape
                   && it_fiducial->fiducial_uid == _pl->get_id())
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
}

shape::delete_bin_button_when_focus_out::delete_bin_button_when_focus_out(
    shape* _shape
) :
    m_shape(_shape)
{
}

//------------------------------------------------------------------------------

bool shape::delete_bin_button_when_focus_out::eventFilter(QObject* /*o*/, QEvent* _e)
{
    if(m_shape->m_bin_button != nullptr && !m_shape->m_bin_button->hasFocus()
       && (_e->type() == QEvent::FocusOut || _e->type() == QEvent::Resize))
    {
        m_shape->m_bin_button->hide();
        delete m_shape->m_bin_button;
        m_shape->m_bin_button = nullptr;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
