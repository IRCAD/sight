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

// cspell:ignore NOLINTNEXTLINE

#include "ruler.hpp"

#include "core/runtime/path.hpp"

#include "data/fiducials_series.hpp"

#include <core/com/slots.hxx>
#include <core/tools/uuid.hpp>

#include <data/boolean.hpp>
#include <data/helper/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/point_list.hpp>
#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/camera.hpp>
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

static constexpr std::uint8_t DISTANCE_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

//------------------------------------------------------------------------------

ruler::ruler() noexcept
{
    new_slot(slots::REMOVE_ALL, &ruler::remove_all, this);
    new_slot(slots::REMOVE_DISTANCES, &ruler::remove_distances, this);
    new_slot(slots::REMOVE_DISTANCES_FROM_CURRENT_SLICE, &ruler::remove_distances_from_current_slice, this);
    new_slot(slots::UPDATE_VISIBILITY_FROM_FIELDS, &ruler::update_visibility_from_field, this);
    new_slot(slots::ACTIVATE_DISTANCE_TOOL, &ruler::activate_distance_tool, this);
    new_slot(slots::UPDATE_MODIFIED_DISTANCE, &ruler::update_modified_distance, this);
    new_slot(slots::RESTRICT_TO_CURRENT_SLICE, &ruler::restrict_to_current_slice, this);

    new_signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL);
}

//------------------------------------------------------------------------------

service::connections_t ruler::auto_connections() const
{
    service::connections_t connections;
    connections.push(IMAGE_INOUT, data::image::DISTANCE_MODIFIED_SIG, slots::UPDATE_MODIFIED_DISTANCE);
    connections.push(IMAGE_INOUT, data::image::DISTANCE_REMOVED_SIG, slots::REMOVE_DISTANCES);
    connections.push(IMAGE_INOUT, data::image::DISTANCE_DISPLAYED_SIG, UPDATE_VISIBILITY_SLOT);
    connections.push(IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void ruler::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_RADIUS_CONFIG      = CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = CONFIG + "distanceQueryFlags";
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
        m_distance_query_flag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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

void ruler::updating()
{
    this->render_service()->make_current();

    const sight::viz::scene3d::layer::csptr layer = this->layer();

    m_sphere_material->update_shading_mode(data::material::shading_t::phong, layer->num_lights(), false, false);
    m_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
    m_dashed_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
}

//------------------------------------------------------------------------------

void ruler::update_from_fiducials()
{
    std::vector<sight::data::point_list::sptr> plv;

    m_cached_fiducials.clear();

    // First collect the point lists to be added (to prevent deadlocks on function calls)
    {
        const auto image = m_image.lock();

        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            auto fiducials = image_series->get_fiducials()->filter_fiducials(
                data::fiducials_series::shape::ruler
            );

            std::copy(fiducials.begin(), fiducials.end(), std::back_inserter(m_cached_fiducials));
        }
    }

    for(const auto& fiducial : m_cached_fiducials)
    {
        std::string id = fiducial.fiducial_uid.value_or("");
        if( /* Check that the distance hasn't been already inserted */
            m_distances.find(id) == m_distances.end())
        {
            plv.push_back(sight::data::helper::fiducials_series::to_point_list(fiducial));
        }
    }

    // Then create the shapes from the point lists
    for(auto& pl : plv)
    {
        this->create_distance(pl);
    }
}

//------------------------------------------------------------------------------

void ruler::restrict_to_current_slice()
{
    const auto image = m_image.lock();
    if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        const int slice_index = static_cast<int>(sight::data::helper::medical_image::get_slice_index(
                                                     *image_series,
                                                     sight::data::helper::medical_image::orientation_t::axial
        ).value_or(-1));

        if(slice_index < 0)
        {
            SIGHT_ERROR("Invalid slice index. Cannot extract inferred data.");
            return;
        }

        const auto& fiducial_sets = image_series->get_fiducials()->get_fiducial_sets();
        for(const auto& fiducial_set : fiducial_sets)
        {
            for(std::size_t i = 0 ; i < fiducial_set.fiducial_sequence.size() ; i++)
            {
                // Filter by shape
                bool shape_ok = false;
                if(fiducial_set.fiducial_sequence[i].shape_type == data::fiducials_series::shape::ruler)
                {
                    shape_ok = true;
                }

                // Filter by referenced frame number (slice index)
                bool referenced_frame_number_ok = false;
                const auto& sequence            = fiducial_set.referenced_image_sequence;
                if(sequence.has_value()
                   && !sequence->empty()
                   && !(sequence->at(i).referenced_frame_number.empty()))
                {
                    for(const auto& frame_number : sequence->at(i).referenced_frame_number)
                    {
                        if(frame_number == slice_index)
                        {
                            referenced_frame_number_ok = true;
                        }
                    }
                }

                // Hide or show the fiducial according to current slice
                if(shape_ok && referenced_frame_number_ok)
                {
                    this->set_visible(fiducial_set.fiducial_sequence[i].fiducial_uid.value_or(""), true);
                }
                else
                {
                    this->set_visible(fiducial_set.fiducial_sequence[i].fiducial_uid.value_or(""), false);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ruler::stopping()
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

    while(!m_distances.empty())
    {
        this->destroy_distance(m_distances.begin()->first);
    }

    m_event_filter = nullptr;
}

//------------------------------------------------------------------------------

void ruler::remove_all()
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

    const auto image                      = m_image.lock();
    data::vector::sptr distance_list_copy = std::make_shared<data::vector>();
    if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        std::vector<data::fiducials_series::fiducial_set> fiducial_sets =
            image_series->get_fiducials()->get_fiducial_sets();
        for(auto it_fiducial_set = fiducial_sets.begin() ; it_fiducial_set != fiducial_sets.end() ; )
        {
            for(auto it_fiducial = it_fiducial_set->fiducial_sequence.begin() ;
                it_fiducial != it_fiducial_set->fiducial_sequence.end() ; )
            {
                if(it_fiducial->shape_type == data::fiducials_series::shape::ruler)
                {
                    distance_list_copy->push_back(sight::data::helper::fiducials_series::to_point_list(*it_fiducial));
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
    else
    {
        data::vector::sptr distance_list = data::helper::medical_image::get_distances(*image);
        distance_list_copy->shallow_copy(distance_list);
        distance_list->clear();
    }

    for(const data::object::sptr& element : *distance_list_copy)
    {
        auto pl = std::dynamic_pointer_cast<data::point_list>(element);
        SIGHT_ASSERT("All elements in distance image field must be point lists.", pl);
        image->signal<data::image::distance_removed_signal_t>(data::image::DISTANCE_REMOVED_SIG)->async_emit(pl);
    }
}

//------------------------------------------------------------------------------

void ruler::remove_distances()
{
    this->render_service()->make_current();

    const auto image = m_image.lock();

    data::vector::sptr distance_field;
    if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        distance_field = std::make_shared<data::vector>();
        for(const data::fiducials_series::fiducial_set& fiducial_set :
            image_series->get_fiducials()->get_fiducial_sets())
        {
            for(const data::fiducials_series::fiducial& fiducial : fiducial_set.fiducial_sequence)
            {
                if(fiducial.shape_type == data::fiducials_series::shape::ruler)
                {
                    if(data::point_list::sptr pl = sight::data::helper::fiducials_series::to_point_list(fiducial))
                    {
                        distance_field->push_back(pl);
                    }
                }
            }
        }
    }
    else
    {
        distance_field = data::helper::medical_image::get_distances(*image);
    }

    std::vector<core::tools::id::type> found_id;
    if(distance_field)
    {
        for(const auto& object : *distance_field)
        {
            found_id.push_back(object->get_id());
        }
    }
    else
    {
        while(!m_distances.empty())
        {
            this->destroy_distance(m_distances.begin()->first);
        }
    }

    std::vector<core::tools::id::type> current_id;
    for(const auto& [id, _] : m_distances)
    {
        current_id.push_back(id);
    }

    for(const core::tools::id::type& id : current_id)
    {
        if(std::find(found_id.begin(), found_id.end(), id) == found_id.end())
        {
            destroy_distance(id);
        }
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void ruler::remove_distances_from_current_slice()
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }

    data::vector::sptr distance_list_copy = std::make_shared<data::vector>();
    {
        const auto image = m_image.lock();
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            const int slice_index = static_cast<int>(sight::data::helper::medical_image::get_slice_index(
                                                         *image_series,
                                                         sight::data::helper::medical_image::orientation_t::axial
            ).value_or(-1));

            if(slice_index < 0)
            {
                SIGHT_ERROR("Invalid slice index. Cannot extract inferred data.");
                return;
            }

            // Get ruler fiducials ruler on the current slice
            const auto& s_fiducials = image_series->get_fiducials()->filter_fiducials(
                data::fiducials_series::shape::ruler,
                slice_index
            );

            if(!s_fiducials.empty())
            {
                for(const auto& sf : s_fiducials)
                {
                    distance_list_copy->push_back(sight::data::helper::fiducials_series::to_point_list(sf));
                }
            }
        }
        else
        {
            data::vector::sptr distance_list = data::helper::medical_image::get_distances(*image);
            distance_list_copy->shallow_copy(distance_list);
            distance_list->clear();
        }
    }

    for(const data::object::sptr& element : *distance_list_copy)
    {
        auto pl = std::dynamic_pointer_cast<data::point_list>(element);
        SIGHT_ASSERT("All elements in distance image field must be point lists.", pl);
        remove_distance(pl);
    }
}

//------------------------------------------------------------------------------

void ruler::update_visibility_from_field()
{
    const auto image = m_image.lock();

    m_visible = data::helper::medical_image::get_distance_visibility(*image);

    for(const auto& distance : m_distances)
    {
        const distance_data& data = distance.second;
        data.m_sphere1->setVisible(m_visible);
        data.m_sphere2->setVisible(m_visible);
        data.m_line->setVisible(m_visible);
        data.m_dashed_line->setVisible(m_visible);
        data.m_label->set_visible(m_visible);
    }
}

//------------------------------------------------------------------------------

void ruler::set_visible(std::string _id, bool _visible)
{
    this->render_service()->make_current();

    auto it_distance = m_distances.find(_id);

    if(it_distance != m_distances.end())
    {
        const distance_data& data = it_distance->second;
        data.m_sphere1->setVisible(_visible);
        data.m_sphere2->setVisible(_visible);
        data.m_line->setVisible(_visible);
        data.m_dashed_line->setVisible(_visible);
        data.m_label->set_visible(_visible);
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void ruler::set_visible(bool _visible)
{
    this->render_service()->make_current();

    for(const auto& distance : m_distances)
    {
        const distance_data& data = distance.second;
        data.m_sphere1->setVisible(_visible);
        data.m_sphere2->setVisible(_visible);
        data.m_line->setVisible(_visible);
        data.m_dashed_line->setVisible(_visible);
        data.m_label->set_visible(_visible);
    }

    m_visible = _visible;

    this->request_render();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> ruler::get_nearest_picked_position(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();
    const auto result      = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vp_x, vp_y);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return result->second + normal * 0.01F;
    }

    return std::nullopt;
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
        Ogre::RaySceneQuery* const ray_scene_query = scene_mgr->createRayQuery(ray, m_distance_query_flag);
        ray_scene_query->setSortByDistance(true);
        if(!ray_scene_query->execute().empty())
        {
            const Ogre::RaySceneQueryResult& query_result_vect = ray_scene_query->getLastResults();

            // First iterate over the ManualObjects to try to find an already existing matching point
            for(std::size_t qr_idx = 0 ; qr_idx < query_result_vect.size() && !found ; qr_idx++)
            {
                const Ogre::MovableObject* const object = query_result_vect[qr_idx].movable;
                const auto object_type                  = object->getMovableType();

                if(object_type == "ManualObject" && object->isVisible())
                {
                    const Ogre::Real scale = 1.15F;
                    for(auto& distance : m_distances)
                    {
                        distance_data& distance_data = distance.second;
                        if(distance_data.m_sphere1 == object)
                        {
                            distance_data.m_node1->setScale(scale, scale, scale);
                            m_picked_data = {&distance_data, true};
                            found         = true;

                            set_cursor(Qt::ClosedHandCursor);
                        }

                        if(distance_data.m_sphere2 == object)
                        {
                            distance_data.m_node2->setScale(scale, scale, scale);
                            m_picked_data = {&distance_data, false};
                            found         = true;

                            set_cursor(Qt::ClosedHandCursor);
                        }
                    }
                }
            }

            // Otherwise, we might have to create a new distance
            if(!found)
            {
                for(std::size_t qr_idx = 0 ; qr_idx < query_result_vect.size() && !found ; qr_idx++)
                {
                    const Ogre::MovableObject* const object = query_result_vect[qr_idx].movable;
                    const auto object_type                  = object->getMovableType();

                    if(object_type == "Entity" && object->isVisible())
                    {
                        //First point
                        auto first_point      = std::make_shared<data::point>();
                        auto clicked_position = this->get_nearest_picked_position(_x, _y);

                        if(clicked_position.has_value())
                        {
                            first_point->set_coord({clicked_position->x, clicked_position->y, clicked_position->z});
                            //Second Point
                            auto second_point = std::make_shared<data::point>();
                            second_point->set_coord({clicked_position->x, clicked_position->y, clicked_position->z});
                            m_points.push_back(first_point);
                            m_points.push_back(second_point);

                            //create_distance equal to 0, firstPoint = secondPoint
                            auto point_list = std::make_shared<data::point_list>();
                            point_list->set_points(m_points);
                            this->create_distance(point_list);
                            this->update_image_distance_field(point_list);
                            auto& distance_data = m_distances[point_list->get_id()];
                            m_picked_data = {&distance_data, false};

                            //remember that this is a creation.
                            m_creation_mode = true;

                            set_cursor(Qt::ClosedHandCursor);

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

        if(m_picked_data.m_data != nullptr)
        {
            // Discard the current distance to launch the ray over the scene without picking this one.
            m_picked_data.m_data->m_sphere1->setQueryFlags(0x0);
            m_picked_data.m_data->m_sphere2->setQueryFlags(0x0);

            const auto layer              = this->layer();
            const Ogre::Camera* const cam = layer->get_default_camera();
            SIGHT_ASSERT("No camera found", cam);

            bool move_in_camera_plane = true;

            Ogre::Vector3 new_pos;
            if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
            {
                // If something is picked, we will snap the landmark to it
                std::optional<Ogre::Vector3> picked_pos = this->get_nearest_picked_position(_x, _y);
                if(picked_pos.has_value())
                {
                    new_pos              = picked_pos.value();
                    move_in_camera_plane = false;
                }
            }

            // Else we move the distance along a plane.
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
                if(m_picked_data.m_first)
                {
                    position = m_picked_data.m_data->m_node1->getPosition();
                }
                else
                {
                    position = m_picked_data.m_data->m_node2->getPosition();
                }

                const Ogre::Plane plane(direction, position);

                const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

                if(!hit.first)
                {
                    SIGHT_ERROR("The ray must intersect the plane")
                    return;
                }

                new_pos = ray.getPoint(hit.second);
            }

            // Reset the query flag.
            m_picked_data.m_data->m_sphere1->setQueryFlags(m_distance_query_flag);
            m_picked_data.m_data->m_sphere2->setQueryFlags(m_distance_query_flag);

            if(m_picked_data.m_first)
            {
                const Ogre::Vector3 second_pos = m_picked_data.m_data->m_node2->getPosition();
                this->update_distance(m_picked_data.m_data, new_pos, second_pos);
            }
            else
            {
                const Ogre::Vector3 first_pos = m_picked_data.m_data->m_node1->getPosition();
                this->update_distance(m_picked_data.m_data, first_pos, new_pos);
            }

            this->request_render();

            const auto image = m_image.lock();
            const auto sig   = image->signal<data::image::distance_modified_signal_t>(
                data::image::DISTANCE_MODIFIED_SIG
            );
            sig->async_emit(m_picked_data.m_data->m_point_list);
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
            Ogre::RaySceneQuery* const ray_scene_query = scene_mgr->createRayQuery(ray, m_distance_query_flag);
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
                        for(auto& distance : m_distances)
                        {
                            distance_data& distance_data = distance.second;
                            if(distance_data.m_sphere1 == object || distance_data.m_sphere2 == object)
                            {
                                set_cursor(Qt::OpenHandCursor);
                                m_is_over_distance = true;
                                found              = true;
                                break;
                            }
                        }
                    }
                }

                if(m_is_over_distance && !found)
                {
                    m_is_over_distance = false;
                    set_cursor(Qt::CrossCursor);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ruler::button_release_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(_button == left && m_picked_data.m_data != nullptr)
    {
        const Ogre::Real scale = 1.F;
        m_picked_data.m_data->m_node1->setScale(scale, scale, scale);
        m_picked_data.m_data->m_node2->setScale(scale, scale, scale);

        const auto pl                     = m_picked_data.m_data->m_point_list;
        const std::array<double, 3> front = pl->get_points().front()->get_coord();
        const std::array<double, 3> back  = pl->get_points().back()->get_coord();

        const Ogre::Vector3 begin(static_cast<float>(front[0]),
                                  static_cast<float>(front[1]),
                                  static_cast<float>(front[2]));
        const Ogre::Vector3 end(static_cast<float>(back[0]),
                                static_cast<float>(back[1]),
                                static_cast<float>(back[2]));

        const int length = static_cast<int>(std::round((end - begin).length()));

        // if it is in creation mode, and a distance is calculated, it is the second point of the distance.
        // Finish the distance creation
        if(m_creation_mode && length != 0)
        {
            m_creation_mode = false;
            set_cursor(Qt::OpenHandCursor);
            m_picked_data = {nullptr, true};
            m_points.clear();
        }
        //If it is not a creationMode, a distance null means that the distance should be removed
        else if(!m_creation_mode)
        {
            if(length == 0)
            {
                destroy_distance(pl->get_id());
                remove_distance(pl);
                set_cursor(Qt::CrossCursor);
                m_picked_data = {nullptr, true};
                m_points.clear();
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
                    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
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
                        m_picked_data.m_first ? m_picked_data.m_data->m_node1 : m_picked_data.m_data->m_node2;
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
                        [this, pl = m_picked_data.m_data->m_point_list]
                        {
                            m_bin_button->hide();
                            destroy_distance(pl->get_id());
                            remove_distance(pl);
                            set_cursor(Qt::CrossCursor);
                            m_picked_data = {nullptr, true};
                            m_points.clear();

                            delete m_bin_button;
                            m_bin_button = nullptr;
                        });
                }

                set_cursor(Qt::OpenHandCursor);
                m_picked_data = {nullptr, true};
                m_points.clear();
            }
        }

        this->layer()->request_render();
    }
    else if(_button == right)
    {
        // right button in creation mode destroys the created distance
        if(m_creation_mode)
        {
            m_creation_mode = false;
            const auto pl = m_picked_data.m_data->m_point_list;
            destroy_distance(pl->get_id());
            remove_distance(pl);
            set_cursor(Qt::CrossCursor);
            m_picked_data = {nullptr, true};
            m_points.clear();
        }
        //right button other than in creation mode goes out of the add distance mode
        else
        {
            activate_distance_tool(false);
            this->signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL)->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void ruler::key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_tool_activated && _key == Qt::Key_Escape)
    {
        activate_distance_tool(false);
        this->signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL)->async_emit();
    }
}

//------------------------------------------------------------------------------

void ruler::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    if(m_bin_button != nullptr)
    {
        m_bin_button->hide();
        delete m_bin_button;
        m_bin_button = nullptr;
    }
}

//------------------------------------------------------------------------------

void ruler::create_distance(data::point_list::sptr& _pl)
{
    const core::tools::id::type id = _pl->get_id();
    SIGHT_ASSERT("The distance already exist", m_distances.find(id) == m_distances.end());

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

    const std::array<double, 3> front = _pl->get_points().front()->get_coord();
    const std::array<double, 3> back  = _pl->get_points().back()->get_coord();

    const Ogre::Vector3 begin = Ogre::Vector3(
        static_cast<float>(front[0]),
        static_cast<float>(front[1]),
        static_cast<float>(front[2])
    );
    const Ogre::Vector3 end = Ogre::Vector3(
        static_cast<float>(back[0]),
        static_cast<float>(back[1]),
        static_cast<float>(back[2])
    );

    // First sphere.
    Ogre::ManualObject* const sphere1 = scene_mgr->createManualObject(this->get_id() + "_sphere1_" + id);
    sight::viz::scene3d::helper::manual_object::create_sphere(
        sphere1,
        m_sphere_material_name,
        *colour,
        m_sphere_radius
    );
    sphere1->setQueryFlags(m_distance_query_flag);
    // Render this sphere over all others objects.
    sphere1->setRenderQueueGroup(DISTANCE_RQ_GROUP_ID);
    SIGHT_ASSERT("Can't create the first entity", sphere1);
    Ogre::SceneNode* const node1 = root_node->createChildSceneNode(this->get_id() + "_node1_" + id, begin);
    SIGHT_ASSERT("Can't create the first node", node1);
    node1->attachObject(sphere1);

    // Second sphere.
    Ogre::ManualObject* const sphere2 = scene_mgr->createManualObject(this->get_id() + "_sphere2_" + id);
    sight::viz::scene3d::helper::manual_object::create_sphere(
        sphere2,
        m_sphere_material_name,
        *colour,
        m_sphere_radius
    );
    sphere2->setQueryFlags(m_distance_query_flag);
    // Render this sphere over all others objects.
    sphere2->setRenderQueueGroup(DISTANCE_RQ_GROUP_ID);
    SIGHT_ASSERT("Can't create the second entity", sphere2);
    Ogre::SceneNode* const node2 = root_node->createChildSceneNode(this->get_id() + "_node2_" + id, end);
    SIGHT_ASSERT("Can't create the second node", node2);
    node2->attachObject(sphere2);

    // Line.
    Ogre::ManualObject* const line = scene_mgr->createManualObject(this->get_id() + "_line_" + id);
    SIGHT_ASSERT("Can't create the line", line);
    line->begin(m_line_material_name, Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
    line->colour(*colour);
    line->position(begin);
    line->position(end);
    line->end();
    line->setQueryFlags(0x0);
    root_node->attachObject(line);

    // Dashed line.
    Ogre::ManualObject* const dashed_line = scene_mgr->createManualObject(this->get_id() + "_dashedLine_" + id);
    // FIXME Currently breaking the usage of rulers
    // SIGHT_ASSERT("Can't create the dashed line", dashed_line);
    // dashed_line->begin(
    //     m_dashed_line_material_name,
    //     Ogre::RenderOperation::OT_LINE_LIST,
    //     sight::viz::scene3d::RESOURCE_GROUP
    // );
    // sight::viz::scene3d::helper::manual_object::draw_dashed_line(
    //     dashed_line,
    //     begin,
    //     end,
    //     m_sphere_radius * m_spacing.x * 2.0F,
    //     m_sphere_radius * m_spacing.x * 1.5F,
    //     colour
    // );
    // dashed_line->end();
    dashed_line->setQueryFlags(0x0);
    // Render this line over all others objects.
    dashed_line->setRenderQueueGroup(DISTANCE_RQ_GROUP_ID);
    root_node->attachObject(dashed_line);

    // Label.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    sight::viz::scene3d::text::sptr label = sight::viz::scene3d::text::make(layer);

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = sight::viz::scene3d::helper::scene::get_length(begin, end);
    label->set_text(length);
    label->set_text_color(*colour);
    label->set_font_size(m_font_size);
    Ogre::SceneNode* const label_node = root_node->createChildSceneNode(this->get_id() + "_labelNode_" + id, end);
    SIGHT_ASSERT("Can't create the label node", label_node);
    label->attach_to_node(label_node, this->layer()->get_default_camera());

    // Set the visibility.
    sphere1->setVisible(m_visible);
    sphere2->setVisible(m_visible);
    line->setVisible(m_visible);
    dashed_line->setVisible(m_visible);
    label->set_visible(m_visible);

    // Store data in the map.
    const distance_data data {_pl, node1, sphere1, node2, sphere2, line, dashed_line, label_node, label};
    m_distances[id] = data;
}

//------------------------------------------------------------------------------

void ruler::update_image_distance_field(data::point_list::sptr _pl)
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
            fiducial.shape_type           = data::fiducials_series::shape::ruler;
            fiducial.fiducial_identifier  = _pl->get_id();
            fiducial.fiducial_description = "Distance";
            fiducial.fiducial_uid         = _pl->get_id();
            std::array<double, 3> first_point  = _pl->get_points().front()->get_coord();
            std::array<double, 3> second_point = _pl->get_points().back()->get_coord();
            fiducial.contour_data = {
                {.x = first_point[0], .y = first_point[1], .z = first_point[2]},
                {.x = second_point[0], .y = first_point[1], .z = first_point[2]}
            };
            // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too
            // lazy to do that, so I simply get rid of GraphicCoordinatesDataSequence.
            fiducial.graphic_coordinates_data_sequence = std::nullopt;
            fiducial_set.fiducial_sequence.push_back(fiducial);

            // Fill in the referenced_image with the slice index to easily retrieve fiducials related to a specific
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

            image_series->get_fiducials()->append_fiducial_set(fiducial_set);
        }
        else
        {
            data::vector::sptr distances_field = data::helper::medical_image::get_distances(*image);

            if(!distances_field)
            {
                distances_field = std::make_shared<data::vector>();
                distances_field->push_back(_pl);
                data::helper::medical_image::set_distances(*image, distances_field);
            }
            else
            {
                distances_field->push_back(_pl);
                data::helper::medical_image::set_distances(*image, distances_field);
            }
        }
    }
}

//------------------------------------------------------------------------------

void ruler::update_distance(
    const distance_data* const _data,
    Ogre::Vector3 _begin,
    Ogre::Vector3 _end
)
{
    SIGHT_ASSERT("Distance can't be null", _data);

    // Update spheres position.
    _data->m_node1->setPosition(_begin);
    _data->m_node2->setPosition(_end);

    // Update the line.
    Ogre::ManualObject* const line = _data->m_line;
    line->beginUpdate(0);
    line->position(_begin);
    line->position(_end);
    line->end();

    // Update the label.

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = sight::viz::scene3d::helper::scene::get_length(_begin, _end);
    _data->m_label->set_text(length);
    _data->m_label_node->setPosition(_end);

    // FIXME Currently breaking the usage of rulers
    // Update the dashed line
    // Ogre::ManualObject* const dashed_line = _data->m_dashed_line;
    // dashed_line->beginUpdate(0);
    // sight::viz::scene3d::helper::manual_object::draw_dashed_line(
    //     dashed_line,
    //     _begin,
    //     _end,
    //     m_sphere_radius * m_spacing.x * 2.0F,
    //     m_sphere_radius * m_spacing.x * 1.5F
    // );
    // dashed_line->end();

    // Update the field data.
    const data::mt::locked_ptr lock(_data->m_point_list);
    _data->m_point_list->get_points().front()->set_coord({_begin[0], _begin[1], _begin[2]});
    _data->m_point_list->get_points().back()->set_coord({_end[0], _end[1], _end[2]});

    {
        const auto image = m_image.lock();
        if(auto image_series = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            // With fields, the modified point list is a shared pointer to the distance field, so there is nothing more
            // to do; however this isn't enough for fiducials, which must be updated manually.
            std::vector<data::fiducials_series::fiducial_set> fiducial_sets =
                image_series->get_fiducials()->get_fiducial_sets();
            for(data::fiducials_series::fiducial_set& fiducial_set : fiducial_sets)
            {
                for(data::fiducials_series::fiducial& fiducial : fiducial_set.fiducial_sequence)
                {
                    if(fiducial.shape_type == data::fiducials_series::shape::ruler
                       && fiducial.fiducial_uid == _data->m_point_list->get_id())
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

    const auto& sig_modified = _data->m_point_list->signal<data::point_list::modified_signal_t>(
        data::point_list::MODIFIED_SIG
    );

    core::com::connection::blocker blocker(sig_modified->get_connection(slot(service::slots::UPDATE)));
    sig_modified->async_emit();

    this->request_render();
}

//------------------------------------------------------------------------------

void ruler::destroy_distance(core::tools::id::type _id)
{
    const auto it = m_distances.find(_id);
    SIGHT_ASSERT("The distance is not found", it != m_distances.end());

    // Destroy Ogre resource.
    distance_data distance_data         = it->second;
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    distance_data.m_label->detach_from_node();
    distance_data.m_label->set_visible(false);
    distance_data.m_label.reset();
    scene_mgr->destroySceneNode(distance_data.m_node1);
    scene_mgr->destroyManualObject(distance_data.m_sphere1);
    scene_mgr->destroySceneNode(distance_data.m_node2);
    scene_mgr->destroyManualObject(distance_data.m_sphere2);
    scene_mgr->destroyManualObject(distance_data.m_line);
    scene_mgr->destroyManualObject(distance_data.m_dashed_line);
    scene_mgr->destroySceneNode(distance_data.m_label_node);

    // Remove it from the map.
    m_distances.erase(it);
}

//------------------------------------------------------------------------------

void ruler::activate_distance_tool(bool _activate)
{
    if(_activate)
    {
        m_tool_activated = true;
        m_picked_data    = {nullptr, true};
        set_cursor(Qt::CrossCursor);
        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        m_event_filter = std::make_unique<delete_bin_button_when_focus_out>(this);
        parent_widget->installEventFilter(m_event_filter.get());
    }
    else
    {
        m_tool_activated   = false;
        m_is_over_distance = false;
        m_creation_mode    = false;
        m_picked_data      = {nullptr, true};
        m_points.clear();

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
}

//------------------------------------------------------------------------------

void ruler::update_modified_distance(data::point_list::sptr _pl)
{
    if(m_distances.find(_pl->get_id()) == m_distances.end())
    {
        // create Distance if it doesn't exist
        this->create_distance(_pl);
    }
    else
    {
        // if it already exists, update distance with the new position
        auto distance_to_update = m_distances[_pl->get_id()];

        const std::array<double, 3> front = _pl->get_points().front()->get_coord();
        const std::array<double, 3> back  = _pl->get_points().back()->get_coord();

        const Ogre::Vector3 begin = Ogre::Vector3(
            static_cast<float>(front[0]),
            static_cast<float>(front[1]),
            static_cast<float>(front[2])
        );
        const Ogre::Vector3 end = Ogre::Vector3(
            static_cast<float>(back[0]),
            static_cast<float>(back[1]),
            static_cast<float>(back[2])
        );
        this->update_distance(&distance_to_update, begin, end);
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

void ruler::remove_distance(data::point_list::sptr _pl)
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
    else
    {
        const auto vect_distance = data::helper::medical_image::get_distances(*image);
        const auto new_vec       = std::remove(vect_distance->begin(), vect_distance->end(), _pl);
        vect_distance->erase(new_vec, vect_distance->end());
    }

    const auto sig = image->signal<data::image::distance_removed_signal_t>(
        data::image::DISTANCE_REMOVED_SIG
    );
    sig->async_emit(_pl);
}

ruler::delete_bin_button_when_focus_out::delete_bin_button_when_focus_out(
    ruler* _ruler
) :
    m_ruler(_ruler)
{
}

//------------------------------------------------------------------------------

bool ruler::delete_bin_button_when_focus_out::eventFilter(QObject* /*o*/, QEvent* _e)
{
    if(m_ruler->m_bin_button != nullptr && !m_ruler->m_bin_button->hasFocus()
       && (_e->type() == QEvent::FocusOut || _e->type() == QEvent::Resize))
    {
        m_ruler->m_bin_button->hide();
        delete m_ruler->m_bin_button;
        m_ruler->m_bin_button = nullptr;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
