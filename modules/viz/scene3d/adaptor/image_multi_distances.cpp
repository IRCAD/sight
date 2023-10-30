/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/image_multi_distances.hpp"

#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/material.hpp>
#include <data/point_list.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OgreEntity.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t REMOVE_DISTANCES_SLOT              = "removeDistances";
static const core::com::slots::key_t UPDATE_VISIBILITY_FROM_FIELDS_SLOT = "updateVisibilityFromField";
static const core::com::slots::key_t ACTIVATE_DISTANCE_TOOL_SLOT        = "activateDistanceTool";
static const core::com::slots::key_t UPDATE_MODIFIED_DISTANCE_SLOT      = "updateModifiedDistance";

static constexpr std::uint8_t DISTANCE_RQ_GROUP_ID = sight::viz::scene3d::rq::SURFACE_ID;

//------------------------------------------------------------------------------

Ogre::ColourValue image_multi_distances::generate_color()
{
    if(std::getenv("GUI_TESTS_ARE_RUNNING") != nullptr)
    {
        // on windows and linux, the color is not the same and prevent comparison with a reference image in GUI tests.
        // For that reason, the color is fixed in gui tests.
        return Ogre::ColourValue(236 / 255.0F, 219 / 255.0F, 84 / 255.0F);
    }

    ++m_color_index;
    switch(m_color_index % 7)
    {
        case 0:
            return Ogre::ColourValue(63 / 255.0F, 105 / 255.0F, 170 / 255.0F);

        case 1:
            return Ogre::ColourValue(249 / 255.0F, 103 / 255.0F, 20 / 255.0F);

        case 2:
            return Ogre::ColourValue(236 / 255.0F, 219 / 255.0F, 84 / 255.0F);

        case 3:
            return Ogre::ColourValue(233 / 255.0F, 75 / 255.0F, 60 / 255.0F);

        case 4:
            return Ogre::ColourValue(121 / 255.0F, 199 / 255.0F, 83 / 255.0F);

        case 5:
            return Ogre::ColourValue(149 / 255.0F, 222 / 255.0F, 227 / 255.0F);

        case 6:
        default:
            return Ogre::ColourValue(29 / 255.0F, 45 / 255.0F, 168 / 255.0F);
    }
}

//------------------------------------------------------------------------------

Ogre::Vector3 image_multi_distances::get_cam_direction(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//------------------------------------------------------------------------------

void image_multi_distances::generate_dashed_line(
    Ogre::ManualObject* const _object,
    const Ogre::Vector3& _begin,
    const Ogre::Vector3& _end,
    float _thickness
)
{
    const Ogre::Vector3 dashed_line_dir = (_end - _begin);
    const float len                     = dashed_line_dir.length();
    Ogre::Vector3 dashed_line_dir_n     = (_end - _begin);
    dashed_line_dir_n.normalise();

    Ogre::Vector3 dashed_line_pos = _begin;
    for(std::size_t i = 0 ; i <= static_cast<std::size_t>((len - _thickness * 1.5) / (_thickness * 2)) ; i++)
    {
        _object->position(dashed_line_pos);
        dashed_line_pos += dashed_line_dir_n * _thickness;
        _object->position(dashed_line_pos);
        dashed_line_pos += dashed_line_dir_n * _thickness;
    }

    _object->end();
}

//------------------------------------------------------------------------------

std::string image_multi_distances::get_length(const Ogre::Vector3& _begin, const Ogre::Vector3& _end)
{
    const int length = static_cast<int>(std::round((_end - _begin).length()));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

image_multi_distances::image_multi_distances() noexcept
{
    SIGHT_WARN(
        "'sight::module::viz::scene3d::adaptor::image_multi_distances' is deprecated, please use"
        " 'sight::module::viz::scene3d_qt::adaptor::image_multi_distances' instead."
    );

    new_slot(REMOVE_DISTANCES_SLOT, &image_multi_distances::remove_distances, this);
    new_slot(UPDATE_VISIBILITY_FROM_FIELDS_SLOT, &image_multi_distances::update_visibility_from_field, this);
    new_slot(ACTIVATE_DISTANCE_TOOL_SLOT, &image_multi_distances::activate_distance_tool, this);
    new_slot(UPDATE_MODIFIED_DISTANCE_SLOT, &image_multi_distances::update_modified_distance, this);

    new_signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL);
}

//------------------------------------------------------------------------------

service::connections_t image_multi_distances::auto_connections() const
{
    connections_t connections;
    connections.push(IMAGE_INOUT, data::image::DISTANCE_MODIFIED_SIG, UPDATE_MODIFIED_DISTANCE_SLOT);
    connections.push(IMAGE_INOUT, data::image::DISTANCE_REMOVED_SIG, REMOVE_DISTANCES_SLOT);
    connections.push(IMAGE_INOUT, data::image::DISTANCE_DISPLAYED_SIG, UPDATE_VISIBILITY_SLOT);
    connections.push(IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void image_multi_distances::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_RADIUS_CONFIG      = CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = CONFIG + "distanceQueryFlags";

    m_font_size              = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_distance_sphere_radius = config.get<float>(s_RADIUS_CONFIG, m_distance_sphere_radius);
    m_interactive            = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority               = config.get<int>(s_PRIORITY_CONFIG, m_priority);

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

void image_multi_distances::starting()
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
}

//------------------------------------------------------------------------------

void image_multi_distances::updating()
{
    this->render_service()->make_current();

    const sight::viz::scene3d::layer::csptr layer = this->layer();

    m_sphere_material->update_shading_mode(data::material::shading_t::phong, layer->num_lights(), false, false);
    m_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
    m_dashed_line_material->update_shading_mode(data::material::shading_t::ambient, layer->num_lights(), false, false);
}

//------------------------------------------------------------------------------

void image_multi_distances::stopping()
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
}

//------------------------------------------------------------------------------

void image_multi_distances::remove_distances()
{
    this->render_service()->make_current();

    const auto image = m_image.lock();

    const data::vector::csptr distance_field = data::helper::medical_image::get_distances(*image);

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

void image_multi_distances::update_visibility_from_field()
{
    this->render_service()->make_current();

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

    this->request_render();
}

//------------------------------------------------------------------------------

void image_multi_distances::set_visible(bool _visible)
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

    this->request_render();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> image_multi_distances::get_nearest_picked_position(int _x, int _y)
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

void image_multi_distances::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    if(_button == left && m_tool_activated && !m_creation_mode)
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

                if(object_type == "Entity" && object->isVisible())
                {
                    //First point
                    auto first_point      = std::make_shared<data::point>();
                    auto clicked_position = this->get_nearest_picked_position(_x, _y);
                    first_point->set_coord({clicked_position->x, clicked_position->y, clicked_position->z});
                    //Second Point
                    auto second_point = std::make_shared<data::point>();
                    second_point->set_coord({clicked_position->x, clicked_position->y, clicked_position->z});
                    m_points.push_back(first_point);
                    m_points.push_back(second_point);

                    //createDistance equal to 0, firstPoint = secondPoint
                    auto point_list = std::make_shared<data::point_list>();
                    point_list->set_points(m_points);
                    this->create_distance(point_list);
                    this->update_image_distance_field(point_list);
                    auto& distance_data = m_distances[point_list->get_id()];
                    m_picked_data = {&distance_data, false};

                    //remember that this is a creation.
                    m_creation_mode = true;

                    sight::ui::cursor cursor;
                    cursor.set_cursor(ui::cursor_base::closed_hand, false);

                    break;
                }

                if(object_type == "manual_object" && object->isVisible())
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

                            sight::ui::cursor cursor;
                            cursor.set_cursor(ui::cursor_base::closed_hand, false);
                            break;
                        }

                        if(distance_data.m_sphere2 == object)
                        {
                            distance_data.m_node2->setScale(scale, scale, scale);
                            m_picked_data = {&distance_data, false};
                            found         = true;

                            sight::ui::cursor cursor;
                            cursor.set_cursor(ui::cursor_base::closed_hand, false);
                            break;
                        }
                    }
                }
            }

            delete ray_scene_query;
            if(found)
            {
                this->layer()->cancel_further_interaction();
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::mouse_move_event(
    mouse_button /*_button*/,
    modifier /*_mods*/,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    if(m_tool_activated)
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
                const Ogre::Vector3 direction = this->get_cam_direction(cam);

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
            this->layer()->cancel_further_interaction();

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

                    if(object_type == "manual_object" && object->isVisible())
                    {
                        for(auto& distance : m_distances)
                        {
                            distance_data& distance_data = distance.second;
                            if(distance_data.m_sphere1 == object || distance_data.m_sphere2 == object)
                            {
                                sight::ui::cursor cursor;
                                cursor.set_cursor(ui::cursor_base::open_hand, false);
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
                    sight::ui::cursor cursor;
                    cursor.set_cursor(ui::cursor_base::cross, false);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::leave_event()
{
    sight::ui::cursor cursor;
    cursor.set_cursor(ui::cursor_base::cross, false);
}

//------------------------------------------------------------------------------

void image_multi_distances::enter_event()
{
    if(m_picked_data.m_data != nullptr)
    {
        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::closed_hand, false);
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::button_release_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
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
            sight::ui::cursor cursor;
            cursor.set_cursor(ui::cursor_base::open_hand, false);
            m_picked_data = {nullptr, true};
            m_points.clear();
        }
        //If it is not a creationMode, a distance null means that the distance should be removed
        else if(!m_creation_mode)
        {
            if(length == 0)
            {
                destroy_distance(pl->get_id());
                {
                    const auto image         = m_image.lock();
                    const auto vect_distance = data::helper::medical_image::get_distances(*image);
                    const auto new_vec       = std::remove(vect_distance->begin(), vect_distance->end(), pl);
                    vect_distance->erase(new_vec, vect_distance->end());
                    const auto sig = image->signal<data::image::distance_removed_signal_t>(
                        data::image::DISTANCE_REMOVED_SIG
                    );
                    sig->async_emit(pl);
                }
                sight::ui::cursor cursor;
                cursor.set_cursor(ui::cursor_base::cross, false);
                m_picked_data = {nullptr, true};
                m_points.clear();
            }
            else
            {
                sight::ui::cursor cursor;
                cursor.set_cursor(ui::cursor_base::open_hand, false);
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
            {
                const auto image         = m_image.lock();
                const auto vect_distance = data::helper::medical_image::get_distances(*image);
                const auto new_vec       = std::remove(vect_distance->begin(), vect_distance->end(), pl);
                vect_distance->erase(new_vec, vect_distance->end());
                const auto sig = image->signal<data::image::distance_removed_signal_t>(
                    data::image::DISTANCE_REMOVED_SIG
                );
                sig->async_emit(pl);
            }
            sight::ui::cursor cursor;
            cursor.set_cursor(ui::cursor_base::cross, false);
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

void image_multi_distances::key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    // code for escape
    //Hardcoded, because scene3D has no access to Qt, and this code is stored in Qt::Key_Escape
    if(m_tool_activated && _key == 16777216)
    {
        activate_distance_tool(false);
        this->signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL)->async_emit();
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::create_distance(data::point_list::sptr& _pl)
{
    const core::tools::id::type id = _pl->get_id();
    SIGHT_ASSERT("The distance already exist", m_distances.find(id) == m_distances.end());

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
    Ogre::SceneNode* const root_node    = scene_mgr->getRootSceneNode();

    // Retrieve data used to create Ogre resources.
    const Ogre::ColourValue colour = image_multi_distances::generate_color();

    const std::array<double, 3> front = _pl->get_points().front()->get_coord();
    const std::array<double, 3> back  = _pl->get_points().back()->get_coord();

    const Ogre::Vector3 begin(static_cast<float>(front[0]),
                              static_cast<float>(front[1]),
                              static_cast<float>(front[2]));
    const Ogre::Vector3 end(static_cast<float>(back[0]),
                            static_cast<float>(back[1]),
                            static_cast<float>(back[2]));

    // First sphere.
    Ogre::ManualObject* const sphere1 = scene_mgr->createManualObject(this->get_id() + "_sphere1_" + id);
    sight::viz::scene3d::helper::manual_object::create_sphere(
        sphere1,
        m_sphere_material_name,
        colour,
        m_distance_sphere_radius
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
        colour,
        m_distance_sphere_radius
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
    line->colour(colour);
    line->position(begin);
    line->position(end);
    line->end();
    line->setQueryFlags(0x0);
    root_node->attachObject(line);

    // Dashed line.
    Ogre::ManualObject* const dashed_line = scene_mgr->createManualObject(this->get_id() + "_dashedLine_" + id);
    SIGHT_ASSERT("Can't create the dashed line", dashed_line);
    dashed_line->begin(
        m_dashed_line_material_name,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    dashed_line->colour(colour);
    image_multi_distances::generate_dashed_line(
        dashed_line,
        begin,
        end,
        m_distance_sphere_radius
    );
    dashed_line->setQueryFlags(0x0);
    // Render this line over all others objects.
    dashed_line->setRenderQueueGroup(DISTANCE_RQ_GROUP_ID);
    root_node->attachObject(dashed_line);

    // Label.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    sight::viz::scene3d::text::sptr label = sight::viz::scene3d::text::make(layer);

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = image_multi_distances::get_length(begin, end);
    label->set_text(length);
    label->set_text_color(colour);
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

void image_multi_distances::update_image_distance_field(data::point_list::sptr _pl)
{
    const auto image = m_image.lock();
    if(data::helper::medical_image::check_image_validity(image.get_shared()))
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

//------------------------------------------------------------------------------

void image_multi_distances::update_distance(
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
    const std::string length = image_multi_distances::get_length(_begin, _end);
    _data->m_label->set_text(length);
    _data->m_label_node->setPosition(_end);

    // Update the dashed line
    Ogre::ManualObject* const dashed_line = _data->m_dashed_line;
    dashed_line->beginUpdate(0);
    image_multi_distances::generate_dashed_line(dashed_line, _begin, _end, m_distance_sphere_radius);

    // Update the field data.
    const data::mt::locked_ptr lock(_data->m_point_list);
    _data->m_point_list->get_points().front()->set_coord({_begin[0], _begin[1], _begin[2]});
    _data->m_point_list->get_points().back()->set_coord({_end[0], _end[1], _end[2]});

    const auto& sig_modified = _data->m_point_list->signal<data::point_list::modified_signal_t>(
        data::point_list::MODIFIED_SIG
    );

    core::com::connection::blocker blocker(sig_modified->get_connection(slot(service::slots::UPDATE)));
    sig_modified->async_emit();

    this->request_render();
}

//------------------------------------------------------------------------------

void image_multi_distances::destroy_distance(core::tools::id::type _id)
{
    const auto it = m_distances.find(_id);
    SIGHT_ASSERT("The distance is not found", it != m_distances.end());

    // Destroy Ogre resource.
    distance_data distance_data         = it->second;
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    distance_data.m_label->detach_from_node();
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

void image_multi_distances::activate_distance_tool(bool _activate)
{
    sight::ui::cursor cursor;
    if(_activate)
    {
        m_tool_activated = true;
        cursor.set_cursor(ui::cursor_base::cross);
    }
    else
    {
        m_tool_activated   = false;
        m_is_over_distance = false;
        m_creation_mode    = false;
        m_picked_data      = {nullptr, true};
        m_points.clear();

        cursor.set_default_cursor();
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::update_modified_distance(data::point_list::sptr _pl)
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

        const Ogre::Vector3 begin(static_cast<float>(front[0]),
                                  static_cast<float>(front[1]),
                                  static_cast<float>(front[2]));
        const Ogre::Vector3 end(static_cast<float>(back[0]),
                                static_cast<float>(back[1]),
                                static_cast<float>(back[2]));
        this->update_distance(&distance_to_update, begin, end);
    }

    this->request_render();
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
