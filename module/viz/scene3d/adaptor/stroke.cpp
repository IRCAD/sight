/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "stroke.hpp"

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <data/tools/color.hpp>

#include <OgreCamera.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

Ogre::Vector3 stroke::get_cam_direction(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

stroke::stroke() noexcept
{
    new_slot(slots::ENABLE, &stroke::enable, this);
    new_slot(slots::CLEAR, &stroke::clear, this);
    new_slot(slots::UNDO, &stroke::undo, this);
    new_slot(slots::VALIDATE, &stroke::validate, this);

    new_signal<signals::disabled_t>(signals::DISABLED);
}

//-----------------------------------------------------------------------------

void stroke::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG         = CONFIG + "priority";
    static const std::string s_LINE_COLOR_CONFIG       = CONFIG + "line_color";
    static const std::string s_EDGE_COLOR_CONFIG       = CONFIG + "edge_color";
    static const std::string s_DOUBLE_CLICK_VALIDATION = CONFIG + "validation_on_double_click";
    static const std::string s_QUERY_MASK_CONFIG       = CONFIG + "queryMask";

    m_priority = config.get<int>(s_PRIORITY_CONFIG, m_priority);

    const auto divide_by255 = [](auto&& _p_h1, auto&& ...)
                              {
                                  return std::divides<>()(std::forward<decltype(_p_h1)>(_p_h1), 255.F);
                              };

    const auto hexa_line_color = config.get<std::string>(s_LINE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> line_color {};
    sight::data::tools::color::hexa_string_to_rgba(hexa_line_color, line_color);
    std::ranges::transform(line_color, m_line_color.ptr(), divide_by255);

    const auto hexa_edge_color = config.get<std::string>(s_EDGE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> edge_color {};
    sight::data::tools::color::hexa_string_to_rgba(hexa_edge_color, edge_color);
    std::ranges::transform(edge_color, m_edge_color.ptr(), divide_by255);

    const std::string hexa_mask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
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
}

//-----------------------------------------------------------------------------

void stroke::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    // Add the interactor to the layer.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    const auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);

    // Create entities.
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    m_stroke_node = scene_mgr->getRootSceneNode()->createChildSceneNode(gen_id("stroke_node"));

    m_stroke           = scene_mgr->createManualObject(gen_id("stroke"));
    m_last_stroke_line = scene_mgr->createManualObject(gen_id("laststroke_line"));
    m_stroke->setRenderQueueGroup(sight::viz::scene3d::rq::OVERLAY);
    m_last_stroke_line->setRenderQueueGroup(sight::viz::scene3d::rq::OVERLAY);

    m_stroke_node->attachObject(m_stroke);
    m_stroke_node->attachObject(m_last_stroke_line);

    m_material_name = gen_id("stroke_material");

    m_material = std::make_unique<sight::viz::scene3d::material::standard>(m_material_name);
    m_material->set_layout(sight::data::mesh::attribute::point_colors);
    m_material->set_shading(sight::data::material::shading_t::ambient, layer->num_lights(), false, false);
}

//-----------------------------------------------------------------------------

void stroke::updating()
{
}

//-----------------------------------------------------------------------------

void stroke::stopping()
{
    this->render_service()->make_current();

    // Remove the interactor from the layer.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    const auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->remove_interactor(interactor);

    // Destroy entities.
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    scene_mgr->destroyManualObject(m_last_stroke_line);
    scene_mgr->destroyManualObject(m_stroke);

    scene_mgr->destroySceneNode(m_stroke_node);

    m_material_name = "";
    m_material.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void stroke::enable(bool _enable)
{
    this->render_service()->make_current();

    m_enable = _enable;

    // Stop the stroke interaction.
    m_interaction_enable_state = false;

    // Clear entities.
    m_stroke_tool_positions.clear();
    m_stroke_edge_positions.clear();
    m_stroke->clear();
    m_last_stroke_line->clear();

    // Send a render request.
    this->request_render();
}

//------------------------------------------------------------------------------

void stroke::clear()
{
    const auto point_list = m_point_list.lock();

    if(not point_list->empty())
    {
        point_list->clear();
        point_list->async_emit(sight::data::signals::MODIFIED);
    }
}

//------------------------------------------------------------------------------

void stroke::undo()
{
    modify_stroke(action::remove);
}

//------------------------------------------------------------------------------

void stroke::validate()
{
    this->render_service()->make_current();

    if(!m_stroke_edge_positions.empty())
    {
        // When coming from touch, mouseReleaseEvent is not always called.
        if(m_left_button_move_state)
        {
            // Add a new point to the stroke edge list.
            m_stroke_edge_positions.push_back(m_stroke_tool_positions.back());
            this->draw_stroke();
        }

        const auto point_list = m_point_list.lock();
        point_list->async_emit(sight::data::signals::MODIFIED);
    }

    // Cancel the left button move state.
    m_left_button_move_state = false;

    this->enable(false);

    this->async_emit(signals::DISABLED);

    // Send a render request.
    this->request_render();
}

//-----------------------------------------------------------------------------

Ogre::Vector3 stroke::get_3d_position(int _x, int _y) const
{
    // Compute the ray.
    sight::viz::scene3d::layer::sptr layer = this->layer();

    Ogre::SceneManager* scene_mgr = layer->get_scene_manager();

    const auto result = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr, true);
    Ogre::Vector3 position;

    if(result.has_value())
    {
        position = result->position;
    }
    else
    {
        const Ogre::Camera* const camera = layer->get_default_camera();
        const Ogre::Viewport* const vp   = camera->getViewport();

        const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        Ogre::Ray ray = camera->getCameraToViewportRay(vp_x, vp_y);

        const Ogre::Vector3 direction = camera->getRealDirection();
        const Ogre::Vector3 cam_pos   = camera->getRealPosition();
        const auto stroke_near_plane  = Ogre::Plane(direction, cam_pos + direction * camera->getNearClipDistance());
        // Launch the ray on the near plane after since the ray must be in front or behind it due to the frustum curve.
        std::pair<bool, Ogre::Real> near_hit = Ogre::Math::intersects(ray, stroke_near_plane);
        if(!near_hit.first)
        {
            ray.setDirection(-ray.getDirection());
            near_hit = Ogre::Math::intersects(ray, stroke_near_plane);
        }

        SIGHT_ASSERT("The ray must hit the plane", near_hit.first);
        position = ray.getPoint(near_hit.second + 0.01F);
    }

    return position;
}

//------------------------------------------------------------------------------

void stroke::modify_stroke(action _action, int _x, int _y)
{
    if(m_enable)
    {
        this->render_service()->make_current();

        const sight::viz::scene3d::layer::sptr layer = this->layer();

        m_interaction_enable_state = true;
        m_left_button_move_state   = false;

        // Cancel others interactions.
        layer->cancel_further_interaction();

        // Get the clicked point in the world space.
        const auto position = this->get_3d_position(_x, _y);

        // Check the interactions.
        if(_action == action::add)
        {
            // Check if the point can be added.
            bool near = false;
            for(const Ogre::Vector3 pos : m_stroke_edge_positions)
            {
                if((position - pos).length() < m_stroke_edge_size)
                {
                    near = true;
                    break;
                }
            }

            // Add the clicked point.
            if(!near)
            {
                m_stroke_tool_positions.push_back(position);
                m_stroke_edge_positions.push_back(position);

                const auto point_list = m_point_list.lock();
                point_list->push_back(std::make_shared<sight::data::point>(position.x, position.y, position.z));
            }
            else
            {
                return;
            }
        }
        else if(_action == action::remove)
        {
            // Remove the last clicked point.
            if(!m_stroke_tool_positions.empty())
            {
                m_stroke_edge_positions.pop_back();
                do
                {
                    m_stroke_tool_positions.pop_back();
                }
                while(!m_stroke_tool_positions.empty()
                      && m_stroke_tool_positions.back() != m_stroke_edge_positions.back());
            }

            // Clear the last line if it's empty.
            if(m_stroke_tool_positions.empty())
            {
                m_interaction_enable_state = false;
                m_last_stroke_line->clear();
                m_stroke->clear();
                this->request_render();
                return;
            }
        }

        // Draw the stroke.
        this->draw_stroke();

        // Draw the last stroke line.
        m_last_stroke_line->clear();

        SIGHT_ASSERT("stroke positions must have at east one point", !m_stroke_tool_positions.empty());

        m_last_stroke_line->begin(
            m_material_name,
            Ogre::RenderOperation::OT_LINE_STRIP,
            sight::viz::scene3d::RESOURCE_GROUP
        );

        m_last_stroke_line->colour(m_line_color);
        m_last_stroke_line->position(m_stroke_tool_positions.back());
        if(_x != -1 && _y != -1)
        {
            m_last_stroke_line->position(position);
        }

        m_last_stroke_line->end();

        // Send a render request.
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void stroke::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    if(m_interaction_enable_state)
    {
        // Don't change the zoom when the tool interaction is enabled.
        const sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->cancel_further_interaction();
    }
}

//-----------------------------------------------------------------------------

void stroke::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    if(_button == mouse_button::left)
    {
        modify_stroke(action::add, _x, _y);
    }
    else if(_button == mouse_button::right)
    {
        modify_stroke(action::remove, _x, _y);
    }
}

//-----------------------------------------------------------------------------

void stroke::mouse_move_event(
    mouse_button _button,
    modifier /*_mods*/,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    if(!m_interaction_enable_state)
    {
        return;
    }

    render_service()->make_current();

    // Cancel others interactions.
    layer()->cancel_further_interaction();

    // Get the clicked point in the world space.
    const auto position = get_3d_position(_x, _y);

    // Check if the mouse is still on the last point.
    // This should not happen but it's better to check, since adding the same points twice will break everything.
    if(!m_stroke_tool_positions.empty() && m_stroke_tool_positions.back() == position)
    {
        return;
    }

    if(_button == mouse_button::left)
    {
        // Add a new position and draws the stroke.
        m_stroke_tool_positions.push_back(position);
        const auto point_list = m_point_list.lock();
        point_list->push_back(std::make_shared<sight::data::point>(position.x, position.y, position.z));

        draw_stroke();

        // Enable the left button move state.
        m_left_button_move_state = true;
    }

    // Draw the last stroke line.
    SIGHT_ASSERT("stroke positions must have at east one point", !m_stroke_tool_positions.empty());

    m_last_stroke_line->beginUpdate(0);

    m_last_stroke_line->colour(m_line_color);
    m_last_stroke_line->position(m_stroke_tool_positions.back());
    m_last_stroke_line->position(position);

    m_last_stroke_line->end();

    // Send a render request.
    request_render();
}

//-----------------------------------------------------------------------------

void stroke::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_interaction_enable_state && m_left_button_move_state)
    {
        this->render_service()->make_current();

        // Cancel others interactions.
        const sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->cancel_further_interaction();

        // Add a new point to the stroke edge list.
        m_stroke_edge_positions.push_back(m_stroke_tool_positions.back());
        this->draw_stroke();

        // Cancel the left button move state.
        m_left_button_move_state = false;

        // Send a render request.
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void stroke::draw_stroke()
{
    // Clear the previous stroke.
    m_stroke->clear();

    // Draw the stroke line.
    m_stroke->begin(m_material_name, Ogre::RenderOperation::OT_LINE_STRIP, sight::viz::scene3d::RESOURCE_GROUP);
    m_stroke->colour(m_line_color);
    for(const Ogre::Vector3 pos : m_stroke_tool_positions)
    {
        m_stroke->position(pos);
    }

    m_stroke->end();

    // Draw the spheres at the edge of each line.
    const unsigned int sample = 16;
    const auto delta_ring     = static_cast<float>(Ogre::Math::PI / sample);
    const float delta_seg     = 2 * static_cast<float>(Ogre::Math::PI / sample);

    for(const Ogre::Vector3 pos : m_stroke_edge_positions)
    {
        // Begin a new section.
        m_stroke->begin(m_material_name, Ogre::RenderOperation::OT_TRIANGLE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
        m_stroke->colour(m_edge_color);

        Ogre::uint32 index = 0;
        for(unsigned ring = 0 ; ring <= sample ; ++ring)
        {
            const float r0 = m_stroke_edge_size * std::sin(static_cast<float>(ring) * delta_ring);
            const float y0 = m_stroke_edge_size * std::cos(static_cast<float>(ring) * delta_ring);

            for(unsigned seg = 0 ; seg <= sample ; ++seg)
            {
                const float x0 = r0 * std::sin(static_cast<float>(seg) * delta_seg);
                const float z0 = r0 * std::cos(static_cast<float>(seg) * delta_seg);
                Ogre::Vector3 point(x0, y0, z0);

                m_stroke->position(pos + point);

                if(ring != sample)
                {
                    m_stroke->index(index + sample + 1);
                    m_stroke->index(index);
                    m_stroke->index(index + sample);
                    m_stroke->index(index + sample + 1);
                    m_stroke->index(index + 1);
                    m_stroke->index(index);
                    ++index;
                }
            }
        }

        m_stroke->end();
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
